#pragma once

#include <atomic>
#include <tuple>
#include <vector>

#include "../mark_pointer.hpp"
#include "../output.hpp"
#include "../debug.hpp"

#include "default_destructor.hpp"
#include "reclamation_guard.hpp"

namespace utils_tm
{
namespace reclamation_tm
{

    namespace otm = out_tm;
    namespace dtm = debug_tm;

    template<class T,
             class Destructor = default_destructor<T>,
             size_t maxThreads=64, size_t maxProtections=256>
    class hazard_manager
    {
    private:
        using this_type           = hazard_manager<T, Destructor,
                                                   maxThreads, maxProtections>;
    public:
        using pointer_type        = T*;
        using atomic_pointer_type = std::atomic<T*>;

        hazard_manager(const Destructor& destructor = Destructor())
            : _destructor(destructor), _handle_counter(-1)
        { for (auto& a : _handles) a.store(nullptr); }
        hazard_manager(const hazard_manager&) = delete;
        hazard_manager& operator=(const hazard_manager&) = delete;
        hazard_manager(hazard_manager&& other) = delete;
        hazard_manager& operator=(hazard_manager&& other) = delete;
        ~hazard_manager();

        struct internal_handle
        {
            enum class istate
            {
                NOT_FOUND,
                MARKED,
                UNMARKED
            };

            internal_handle() : _counter(0)
            {
                for (size_t i = 0; i < maxProtections; ++i)
                    _ptr[i].store(nullptr);
            }

            std::atomic_int               _counter;
            atomic_pointer_type           _ptr[maxProtections];

            inline int                   insert(pointer_type ptr);
            inline std::pair<istate,int> remove(pointer_type ptr);
            inline istate                replace(int i, pointer_type ptr);

            // has to work concurrently
            inline istate                mark  (pointer_type ptr, int pos = 1);
            inline int                   find  (pointer_type ptr) const;
            void                         print () const;
        };

        class handle_type
        {
        private:
            using parent_type = hazard_manager<T,Destructor,
                                               maxThreads,maxProtections>;
            using this_type   = handle_type;

            using istate      = typename internal_handle::istate;

        public:
            using pointer_type        = typename parent_type::pointer_type;
            using atomic_pointer_type = typename parent_type::atomic_pointer_type;
            using guard_type          = reclamation_guard<T,this_type>;

            handle_type(parent_type& parent, internal_handle& internal, int id);
            handle_type(const handle_type&) = delete;
            handle_type& operator=(const handle_type&) = delete;
            // handles should not be moved while other operations are ongoing
            handle_type(handle_type&& other) noexcept;
            handle_type& operator=(handle_type&& other) noexcept;
            ~handle_type();

            template <class ... Args>
            inline T*   create_pointer(Args&& ... args) const;

            inline T*   protect(atomic_pointer_type& ptr);
            inline void protect_raw(pointer_type ptr);

            inline void unprotect(pointer_type ptr);
            inline void unprotect(std::vector<T*>& vec);

            inline guard_type guard(atomic_pointer_type& ptr);
            inline guard_type guard(pointer_type ptr);

            inline void safe_delete(pointer_type ptr);
            inline void delete_raw(pointer_type ptr);
            inline bool is_safe(pointer_type ptr);



            void print() const;

            size_t n;
        private:
            inline void continue_deletion(pointer_type ptr, int pos = -1);

            parent_type&     _parent;
            internal_handle& _internal;
            int              _id;
        };
        friend handle_type;

        handle_type get_handle();
        void print() const;
    private:
        Destructor                    _destructor;
        std::atomic_int               _handle_counter;
        std::atomic<internal_handle*> _handles[maxThreads];

    };



    template <class T, class D, size_t mt, size_t mp>
    hazard_manager<T,D,mt,mp>::~hazard_manager()
    {
        auto counter = _handle_counter.load();
        for (int i = counter; i >= 0; --i)
        {
            auto temp = _handles[i].load();
            while (!mark::get_mark<0>(temp))
            { /* wait for heandles to be destroyed */ }
        }

        for (int i = counter; i >= 0; --i)
        {
            delete mark::clear(_handles[i].load());
        }
    }

    template <class T, class D, size_t mt, size_t mp>
    typename hazard_manager<T,D,mt,mp>::handle_type
    hazard_manager<T,D,mt,mp>::get_handle()
    {
        internal_handle* temp0 = new internal_handle();
        internal_handle* temp1;

        int i = 0;
        for (; i < int(mt); ++i)
        {
            temp1 = _handles[i].load();

            if (!temp1)
            {
                if (_handles[i].compare_exchange_strong(temp1, temp0))
                {
                    auto b = _handle_counter.load();
                    while (b < i) _handle_counter.compare_exchange_weak(b, i);
                    return handle_type(*this, *temp0, i);
                }
            }
            if (mark::get_mark<0>(temp1))
            {
                // reuse old handle
                if (_handles[i].compare_exchange_strong(temp1, mark::clear(temp1)))
                {
                    delete temp0;
                    return handle_type(*this, *mark::clear(temp1), i);
                }

            }
        }
        otm::out() << "Error: in hazard_manager get_handle -- out of bounds"
                   << std::endl;
        return handle_type(*this, *temp0, -666);
    }

    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::print() const
    {
        otm::out() << "hazard manager print: " << _handle_counter.load()+1 << "handles" << std::endl;
        for (size_t i = 0; i < mt; ++i)
        {
            otm::out() << i << ": " << _handles[i].load() << std::endl;
        }
    }




    // *** INTERNAL HANDLE *****************************************************
    template <class T, class D, size_t mt, size_t mp>
    int hazard_manager<T,D,mt,mp>::internal_handle::insert(pointer_type ptr)
    {
        auto pos = _counter.fetch_add(1);
        dtm::if_debug_critical("Error: in insert -- "
                               "too many protected pointers", pos >= int(mp));
        dtm::if_debug_critical("Error: in insert -- "
                               "below 0 protected pointers", pos < 0);

        _ptr[pos].store(ptr);
        return pos;
    }

    template <class T, class D, size_t mt, size_t mp>
    std::pair<typename hazard_manager<T,D,mt,mp>::internal_handle::istate, int>
    hazard_manager<T,D,mt,mp>::internal_handle::remove(pointer_type ptr)
    {
        auto pos = find(ptr);
        if (pos < 0)
        {
            return std::make_pair(istate::NOT_FOUND, -1);
        }

        auto last_pos = _counter.load()-1;
        dtm::if_debug_critical("Error: in remove -- "
                               "too many protected pointers", pos >= int(mp));
        dtm::if_debug_critical("Error: in remove -- "
                               "below 0 protected pointers", pos < 0);

        if (pos == last_pos)
        {
            _counter.fetch_sub(1);
            auto temp = _ptr[pos].exchange(nullptr);
            dtm::if_debug("Warning: in rec handle remove"
                          " -- removing last element changed",
                          mark::clear(temp) != ptr);
            return std::make_pair((mark::get_mark<0>(temp)) ? istate::MARKED
                                                            : istate::UNMARKED,
                                  pos);
        }

        auto last_ptr = _ptr[last_pos].load();
        auto temp     = _ptr[pos].exchange(last_ptr);

        dtm::if_debug("Warning: in rec handle remove"
                      " -- element changed since call of find",
                      mark::clear(temp) != ptr);

        bool was_marked = mark::get_mark<0>(temp);

        _counter.fetch_sub(1);
        temp = _ptr[last_pos].exchange(nullptr);
        if (last_ptr != temp)
        {
            dtm::if_debug("Warning: in rec handle remove"
                          " -- last element changed", mark::clear(temp) != last_ptr);
            _ptr[pos].store(last_ptr);
        }

        return std::make_pair(was_marked ? istate::MARKED : istate::UNMARKED,
                              pos);
    }

    template <class T, class D, size_t mt, size_t mp>
    typename hazard_manager<T,D,mt,mp>::internal_handle::istate
    hazard_manager<T,D,mt,mp>::internal_handle::replace(int i, pointer_type ptr)
    {
        auto temp = _ptr[i].exchange(ptr);
        return mark::get_mark<0>(temp) ? istate::MARKED : istate::UNMARKED;
    }

            // has to work concurrently
    template <class T, class D, size_t mt, size_t mp>
    typename hazard_manager<T,D,mt,mp>::internal_handle::istate
    hazard_manager<T,D,mt,mp>::internal_handle::mark(pointer_type ptr, int pos)
    {
        auto temp = pos;
        if (pos < 0) temp = _counter.load() -1;
        dtm::if_debug("Error: in mark -- "
                               "pos larger than expected", pos >= int(mp));
        dtm::if_debug("Error: in mark -- "
                               "pos below 0 after load", pos < 0);

        for (int i = temp; i >= 0; i--)
        {
            auto temp = _ptr[i].load();
            if (mark::clear(temp) == ptr)
            {
                // was marked before -> stays
                if (mark::get_mark<0>(temp)) return istate::MARKED;
                // try to mark it
                if (_ptr[i].compare_exchange_strong(temp, mark::mark<0>(ptr)))
                    return istate::UNMARKED;
                // try failed, because it was marked concurrently
                if (temp == mark::mark<0>(ptr)) return istate::MARKED;
                // failed because it was removed for some reason
                // it was either removed, or pushed forward
                // continue to find it ...
            }

        }
        return istate::NOT_FOUND;
    }

    template <class T, class D, size_t mt, size_t mp>
    int hazard_manager<T,D,mt,mp>::internal_handle::find(pointer_type ptr) const
    {
        auto temp = _counter.load() -1;
        dtm::if_debug("Error: in find -- "
                      "too many current elements", temp >= int(mp));
        if (temp >= int(mp))
        {
            std::cout << "this is weird and should not happen" << std::endl
                      << temp << " elements when calling find" << std::endl
                      << this << " this pointer" << std::endl;
        }

        dtm::if_debug("Error: in find -- "
                      "below 0 current elements", temp < -1); // might be 0
        if (temp < -2)
        {
            std::cout << "this is weird and should not happen (2nd)" << std::endl
                      << temp << " elements when calling find" << std::endl
                      << this << " this pointer" << std::endl;
        }

        for (int i = temp; i >= 0; i--)
        {
            if (mark::clear(_ptr[i].load()) == ptr) return i;
        }
        return -1;
    }

    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::internal_handle::print() const
    {

        auto temp = _counter.load();
        std::cout << "counter is:" << temp << std::endl;
        for (int i = temp+2; i >= 0; i--)
        {
            std::cout << _ptr[i] << std::endl;
        };
    }


    // *** HANDLE **************************************************************
    // ***** HANDLE CONSTRUCTORS ***********************************************
    template <class T, class D, size_t mt, size_t mp>
    hazard_manager<T,D,mt,mp>::handle_type::handle_type(parent_type& parent,
                                                      internal_handle& internal,
                                                      int id)
        : n(0), _parent(parent), _internal(internal), _id(id)
    { }

    template <class T, class D, size_t mt, size_t mp>
    hazard_manager<T,D,mt,mp>::handle_type::handle_type(handle_type&& source) noexcept
        : _parent(source._parent), _internal(source._internal), _id(source._id)
    {
        source._id = -1;
    }

    template <class T, class D, size_t mt, size_t mp>
    typename hazard_manager<T,D,mt,mp>::handle_type&
    hazard_manager<T,D,mt,mp>::handle_type::operator=(handle_type&& source) noexcept
    {
        if (&source == this) return *this;
        this->handle_type::~handle_type();
        new (this) handle_type(std::move(source));
        return *this;
    }

    template <class T, class D, size_t mt, size_t mp>
    hazard_manager<T,D,mt,mp>::handle_type::~handle_type()
    {
        if (_id < 0) return;

        for (int i = _internal._counter.load()-1; i >= 0; --i)
        {
            auto temp = _internal._ptr[i].exchange(nullptr);
            if (mark::get_mark<0>(temp))
                continue_deletion(mark::clear(temp), i);
        }
        _internal._counter.store(0);

        _parent._handles[_id].store(mark::mark<0>(&_internal));
    }



    // ***** HANDLE FUNCTIONALITY **********************************************
    template <class T, class D, size_t mt, size_t mp> template <class ... Args>
    T* hazard_manager<T,D,mt,mp>::handle_type::create_pointer(Args&& ... args) const
    {
        return new T(std::forward<Args>(args)...);
    }

    template <class T, class D, size_t mt, size_t mp>
    T* hazard_manager<T,D,mt,mp>::handle_type::protect(atomic_pointer_type& ptr)
    {
        ++n;
        auto temp0 = ptr.load();
        auto pos   = _internal.insert(mark::clear(temp0));
        auto temp1 = ptr.load();
        while (mark::clear(temp0) != mark::clear(temp1))
        {
            auto state = _internal.replace(pos, mark::clear(temp1));
            if (state == istate::MARKED)
                continue_deletion(mark::clear(temp0), pos);
            temp0 = temp1;
            temp1 = ptr.load();
        }
        return temp1;
    }

    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::handle_type::protect_raw(pointer_type ptr)
    {
        ++n;
        _internal.insert(mark::clear(ptr));
    }

    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::handle_type::unprotect(pointer_type ptr)
    {
        --n;
        auto cptr = mark::clear(ptr);
        auto [st, pos] = _internal.remove(cptr);
        dtm::if_debug("Warning: in recl handle unprotect -- pointer not found",
                      st == istate::NOT_FOUND);
        if (st == istate::NOT_FOUND)
        {
            std::cout << "looking for " << cptr << " in:" << std::endl;
            _internal.print();
        }
        if (st == istate::MARKED)
            continue_deletion(cptr, pos);
    }

    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::handle_type::unprotect(std::vector<pointer_type>& vec)
    {
        for (auto ptr : vec)
        {
            unprotect(ptr);
        }
    }

    template <class T, class D, size_t mt, size_t mp>
    typename hazard_manager<T,D,mt,mp>::handle_type::guard_type
    hazard_manager<T,D,mt,mp>::handle_type::guard(atomic_pointer_type& aptr)
    {
        return make_rec_guard(*this, aptr);
    }

    template <class T, class D, size_t mt, size_t mp>
    typename hazard_manager<T,D,mt,mp>::handle_type::guard_type
    hazard_manager<T,D,mt,mp>::handle_type::guard(pointer_type aptr)
    {
        return make_rec_guard(*this, aptr);
    }

    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::handle_type::safe_delete(pointer_type ptr)
    {
        continue_deletion(ptr);
    }

    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::handle_type::delete_raw(pointer_type ptr)
    {
        delete mark::clear(ptr);
    }

    template <class T, class D, size_t mt, size_t mp>
    bool hazard_manager<T,D,mt,mp>::handle_type::is_safe(pointer_type ptr)
    {
        auto cptr = mark::clear(ptr);
        for (int i = _parent._handle_counter.load(); i >= 0; --i)
        {
            auto temp_handle = _parent._handles[i].load();
            if (mark::get_mark<0>(temp_handle)) continue;
            if (temp_handle->find(cptr) != -1)
                return false;
        }
        return true;
    }

    // ***** HANDLE HELPER FUNCTION ********************************************
    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::handle_type::continue_deletion(
        pointer_type ptr,
        int pos)
    {
        auto temp = _internal.mark(ptr, pos);
        if (temp != istate::NOT_FOUND) return;

        for (int i = _id-1; i >=0; --i)
        {
            auto temp_handle = _parent._handles[i].load();
            if (mark::get_mark<0>(temp_handle)) continue;
            if (temp_handle->mark(ptr) != istate::NOT_FOUND) return;
        }

        _parent._destructor.destroy(*this, ptr);
    }

    template <class T, class D, size_t mt, size_t mp>
    void hazard_manager<T,D,mt,mp>::handle_type::print() const
    {
        out_tm::out() << "* print in hazard reclamation handle "
                      << _internal._counter.load()
                      << " pointer protected *"
                      << std::endl;
    }

};
};
