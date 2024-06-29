/*******************************************************************************
 * This file is part of pasta::utils.
 *
 * Copyright (C) 2021 Florian Kurpicz <florian@kurpicz.org>
 *
 * pasta::utils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pasta::utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pasta::utils.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "pasta/utils/benchmark/malloc.hpp"

#include <iostream>

namespace pasta {

  //! Sentinel used to mark monitored memory.
  constexpr size_t MEMORY_SENTINEL = 0x0D15EA5E;

  /*!
   * \brief Header stored in front of allocated memory.
   *
   * The header contains the sentinel (to mark it as memory that is monitored)
   * and the size of the memory (to keep track of the freed memory).
   */
  struct MemoryBlockHeader {
    //! Sentinel that marks monitored memory.
    size_t sentinel;
    //! Size of the allocated memory.
    size_t size;

    MemoryBlockHeader(uint64_t const _sentinel, uint64_t const _size)
      : sentinel(_sentinel), size(_size){ }
  }; // struct MemoryBlockHeader

  void (*MemoryMonitorCallbacks::malloc_callback)(size_t) = nullptr;
  void (*MemoryMonitorCallbacks::free_callback)(size_t) = nullptr;
  void (*MemoryMonitorCallbacks::uncounted_callback)(void) = nullptr;

} // namespace pasta

extern "C" void* malloc(size_t size) {
  if (size == 0) [[unlikely]] {
    return nullptr;
  }

  void* ptr = __libc_malloc(size + sizeof(pasta::MemoryBlockHeader));
  if (ptr == nullptr) [[unlikely]] {
    return ptr;
  }

  auto* mem_block = static_cast<pasta::MemoryBlockHeader*>(ptr);
  mem_block->sentinel = pasta::MEMORY_SENTINEL;
  mem_block->size = size;

  if (pasta::MemoryMonitorCallbacks::malloc_callback != nullptr) [[likely]] {
    pasta::MemoryMonitorCallbacks::malloc_callback(size);
  }

  return static_cast<char*>(ptr) + sizeof(pasta::MemoryBlockHeader);
}

extern "C" void free(void* ptr) {
  if (ptr == nullptr) {
    return;
  }
  auto mem_block = reinterpret_cast<pasta::MemoryBlockHeader*>(
    static_cast<char*>(ptr) - sizeof(pasta::MemoryBlockHeader));
  if (mem_block->sentinel == pasta::MEMORY_SENTINEL) [[likely]] {
    if (pasta::MemoryMonitorCallbacks::free_callback != nullptr) [[likely]] {
      pasta::MemoryMonitorCallbacks::free_callback(mem_block->size);
    }
    __libc_free(mem_block);
  } else [[unlikely]] {
    if (pasta::MemoryMonitorCallbacks::uncounted_callback != nullptr)[[likely]]{
      pasta::MemoryMonitorCallbacks::uncounted_callback();
    }
    __libc_free(ptr);
  }
}

extern "C" void* calloc(size_t number, size_t size) {
  size *= number;
  if (size == 0) {
    return NULL;
  }
  void* ptr = malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

extern "C" void* realloc(void* ptr, size_t size) {
  if (size == 0) [[unlikely]] {
    free(ptr);
    return nullptr;
  } else if (ptr == nullptr) [[unlikely]] {
    return malloc(size);
  } else [[likely]] {
  auto mem_block = reinterpret_cast<pasta::MemoryBlockHeader*>(
    static_cast<char*>(ptr) - sizeof(pasta::MemoryBlockHeader));
    if (mem_block->sentinel == pasta::MEMORY_SENTINEL) [[likely]] {
      size_t const cur_size = mem_block->size;
      if (size <= cur_size) {
	mem_block->size = size;
	return ptr;
      } else {
	void* new_ptr = malloc(size);
	memcpy(new_ptr, ptr, mem_block->size);
	free(mem_block);
	return new_ptr;
      }
    } else [[unlikely]] {
      return __libc_realloc(ptr, size);
    }
  }
}


/******************************************************************************/
