// This file is part of PGM-index <https://github.com/gvinciguerra/PGM-index>.
// Copyright (c) 2018 Giorgio Vinciguerra.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>
#include "OptimalPiecewiseLinearModel.hpp"

#define PGM_SUB_EPS(x, epsilon) ((x) <= (epsilon) ? 0 : ((x) - (epsilon)))
#define PGM_ADD_EPS(x, epsilon, size) ((x) + (epsilon) + 2 >= (size) ? (size) : (x) + (epsilon) + 2)

/**
 * A struct that stores the result of a query to a @ref PGMIndex, that is, a range [@ref lo, @ref hi)
 * centered around an approximate position @ref pos of the sought key.
 */
struct ApproxPos {
    size_t pos; ///< The approximate position of the key.
    size_t lo;  ///< The lower bound of the range.
    size_t hi;  ///< The upper bound of the range.
};

namespace lemonhash::pgm {

/**
 * A space-efficient index that enables fast search operations on a sorted sequence of @c n numbers.
 *
 * A search returns a struct @ref ApproxPos containing an approximate position of the sought key in the sequence and
 * the bounds of a range of size 2*Epsilon+1 where the sought key is guaranteed to be found if present.
 * If the key is not present, the range is guaranteed to contain a key that is not less than (i.e. greater or equal to)
 * the sought key, or @c n if no such key is found.
 * In the case of repeated keys, the index finds the position of the first occurrence of a key.
 *
 * The @p Epsilon template parameter should be set according to the desired space-time trade-off. A smaller value
 * makes the estimation more precise and the range smaller but at the cost of increased space usage.
 *
 * Internally the index uses a succinct piecewise linear mapping from keys to their position in the sorted order.
 * This mapping is represented as a sequence of linear models (segments) which, if @p EpsilonRecursive is not zero, are
 * themselves recursively indexed by other piecewise linear mappings.
 *
 * @tparam K the type of the indexed keys
 * @tparam Epsilon controls the size of the returned search range
 * @tparam EpsilonRecursive controls the size of the search range in the internal structure
 * @tparam Floating the floating-point type to use for slopes
 */
template<typename K, typename Floating = float>
class PGMIndex {
protected:
    struct Segment;

    size_t n;                           ///< The number of elements this index was built on.
    K first_key;                        ///< The smallest element.
    uint16_t epsilon;
    uint16_t epsilon_recursive;
    std::vector<Segment> segments;      ///< The segments composing the index.
    std::vector<size_t> levels_offsets; ///< The starting position of each level in segments[], in reverse order.

    template<typename RandomIt>
    static void build(RandomIt first, RandomIt last,
                      size_t epsilon, size_t epsilon_recursive,
                      std::vector<Segment> &segments,
                      std::vector<size_t> &levels_offsets) {
        auto n = (size_t) std::distance(first, last);
        if (n == 0)
            return;

        levels_offsets.push_back(0);
        segments.reserve(n / (epsilon * epsilon));

        auto ignore_last = *std::prev(last) == std::numeric_limits<K>::max(); // max() is the sentinel value
        auto last_n = n - ignore_last;
        last -= ignore_last;

        auto build_level = [&](auto epsilon, auto in_fun, auto out_fun) {
            auto n_segments = internal::make_segmentation_mod(last_n, epsilon, in_fun, out_fun, false);
            if (last_n > 1 && segments.back().slope == 0) {
                // Here we need to ensure that keys > *(last-1) are approximated to a position == prev_level_size
                segments.emplace_back(*std::prev(last) + 1, 0, last_n);
                ++n_segments;
            }
            segments.emplace_back(last_n); // Add the sentinel segment
            return n_segments;
        };

        // Build first level
        auto in_fun = [&](auto i) { return std::pair<K, size_t>(first[i], i); };
        auto out_fun = [&](auto cs, auto) { segments.emplace_back(cs); };
        last_n = build_level(epsilon, in_fun, out_fun);
        levels_offsets.push_back(levels_offsets.back() + last_n + 1);

        // Build upper levels
        while (epsilon_recursive && last_n > 1) {
            auto offset = levels_offsets[levels_offsets.size() - 2];
            auto in_fun_rec = [&](auto i) { return std::pair<K, size_t>(segments[offset + i].key, i); };
            last_n = build_level(epsilon_recursive, in_fun_rec, out_fun);
            levels_offsets.push_back(levels_offsets.back() + last_n + 1);
        }
    }

    /**
     * Returns the segment responsible for a given key, that is, the rightmost segment having key <= the sought key.
     * @param key the value of the element to search for
     * @return an iterator to the segment responsible for the given key
     */
    auto segment_for_key(const K &key) const {
        if (epsilon_recursive == 0) {
            return std::prev(std::upper_bound(segments.begin(), segments.begin() + segments_count(), key));
        }

        auto it = segments.begin() + *(levels_offsets.end() - 2);
        for (auto l = int(height()) - 2; l >= 0; --l) {
            auto level_begin = segments.begin() + levels_offsets[l];
            auto pos = std::min<size_t>((*it)(key), std::next(it)->intercept);
            auto lo = level_begin + PGM_SUB_EPS(pos, epsilon_recursive + 1u);
            for (; std::next(lo)->key <= key; ++lo)
                continue;
            it = lo;
        }
        return it;
    }

public:

    /**
     * Constructs an empty index.
     */
    PGMIndex() = default;

    /**
     * Constructs the index on the given sorted vector.
     * @param data the vector of keys to be indexed, must be sorted
     */
    explicit PGMIndex(const std::vector<K> &data) : PGMIndex(data.begin(), data.end()) {}

    /**
     * Constructs the index on the sorted keys in the range [first, last).
     * @param first, last the range containing the sorted keys to be indexed
     */
    template<typename RandomIt>
    PGMIndex(RandomIt first, RandomIt last)
        : n(std::distance(first, last)),
          first_key(n ? *first : K(0)),
          segments(),
          levels_offsets() {
        build(first, last, epsilon, epsilon_recursive, segments, levels_offsets);
    }

    /**
     * Returns the approximate position and the range where @p key can be found.
     * @param key the value of the element to search for
     * @return a struct with the approximate position and bounds of the range
     */
    ApproxPos search(const K &key) const {
        auto k = std::max(first_key, key);
        auto it = segment_for_key(k);
        auto pos = std::min<size_t>((*it)(k), std::next(it)->intercept);
        auto lo = PGM_SUB_EPS(pos, epsilon);
        auto hi = PGM_ADD_EPS(pos, epsilon, n);
        return {pos, lo, hi};
    }
    /**
     * Constructs the index on the sorted keys in the range [first, last).
     * @param first, last the range containing the sorted keys to be indexed
     */
    template<typename RandomIt>
    PGMIndex(RandomIt first, RandomIt last, uint16_t epsilon, uint16_t epsilon_recursive = 4)
        : n(std::distance(first, last)),
          first_key(n ? *first : K(0)),
          epsilon(epsilon),
          epsilon_recursive(epsilon_recursive),
          segments(),
          levels_offsets() {
        build(first, last, epsilon, epsilon_recursive, segments, levels_offsets);
    }

    /** Returns the approximate rank of @p key. */
    [[nodiscard]] size_t approximate_rank(const K &key) const {
        auto k = std::max(first_key, key);
        auto it = segment_for_key(k);
        return std::min<size_t>((*it)(k), std::next(it)->intercept);
    }

    /** Execute a given function for each key in the sorted range [first, last). The function takes as the argument
     * an iterator to the current key and the corresponding approximate rank computed by the index. */
    template<typename ForwardIt, typename F>
    void for_each(ForwardIt first, ForwardIt last, F f) const {
        auto segment_it = segments.begin();
        auto it = first;
        while (it != last) {
            if (*it >= std::next(segment_it)->key)
                ++segment_it;
            auto pos = std::min<size_t>((*segment_it)(*it), std::next(segment_it)->intercept);;
            assert(std::abs(int64_t(pos) - int64_t(std::distance(first, it))) <= epsilon + 1);
            f(it, pos);
            ++it;
        }
    }

    [[nodiscard]] size_t epsilon_value() const { return epsilon; }

    /**
     * Returns the number of levels of the index.
     * @return the number of levels of the index
     */
    [[nodiscard]] size_t height() const { return levels_offsets.size() - 1; }

    /**
     * Returns the number of segments in the last level of the index.
     * @return the number of segments
     */
    [[nodiscard]] size_t segments_count() const { return segments.empty() ? 0 : levels_offsets[1] - 1; }

    /**
     * Returns the size of the index in bytes.
     * @return the size of the index in bytes
     */
    [[nodiscard]] size_t size_in_bytes() const { return segments.size() * sizeof(Segment) + levels_offsets.size() * sizeof(size_t); }
};

#pragma pack(push, 1)

template<typename K, typename Floating>
struct PGMIndex<K, Floating>::Segment {
    K key;             ///< The first key that the segment indexes.
    Floating slope;    ///< The slope of the segment.
    int32_t intercept; ///< The intercept of the segment.

    Segment() = default;

    Segment(K key, Floating slope, int32_t intercept) : key(key), slope(slope), intercept(intercept) {};

    explicit Segment(size_t n) : key(std::numeric_limits<K>::max()), slope(), intercept(n) {};

    explicit Segment(const typename internal::OptimalPiecewiseLinearModel<K, size_t>::CanonicalSegment &cs)
        : key(cs.first.x) {
        auto[cs_slope, cs_intercept] = cs.get_floating_point_segment(key);
        if (cs_intercept > std::numeric_limits<decltype(intercept)>::max())
            throw std::overflow_error("Change the type of Segment::intercept to int64");
        slope = cs_slope;
        intercept = cs_intercept;
    }

    friend inline bool operator<(const Segment &s, const K &k) { return s.key < k; }
    friend inline bool operator<(const K &k, const Segment &s) { return k < s.key; }
    friend inline bool operator<(const Segment &s, const Segment &t) { return s.key < t.key; }

    operator K() { return key; };

    /**
     * Returns the approximate position of the specified key.
     * @param k the key whose position must be approximated
     * @return the approximate position of the specified key
     */
    inline size_t operator()(const K &k) const {
        auto pos = int64_t(slope * (k - key)) + intercept;
        return pos > 0 ? size_t(pos) : 0ull;
    }
};

#pragma pack(pop)

}