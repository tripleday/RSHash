#pragma once

#include <stdint.h>
#include <cassert>
#include <cstdint>
#include <bit>
#include <string>
#include <numeric>
#include "util.h"
#include "AlphabetMap.h"

namespace lemonhash {
/** Stores a subset of the alphabet of 7-bit or 8-bit values (depending on whether ASCII is true). */
template<bool ASCII=false, size_t num=9>
struct AlphabetMaps {

    lemonhash::AlphabetMap<ASCII> ams[num];
    size_t max_branching_num;
    int adjust_num=num;

    void set(size_t n, uint8_t c) {
        if constexpr (ASCII)
            assert(c < 128);
        if (n>=num) n=num-1;
        ams[n].set(c);
    }

    public:

        AlphabetMaps() = default;

        /** Constructs from the alphabet of branching characters in the compacted trie of the strings in the given range,
         * which must be sorted lexicographically, by using a precomputed LCP array.
         * If terminator is true, then '\0' is part of the alphabet. */
        AlphabetMaps(const auto lcpsBegin, size_t n, size_t minLCP) {
            for (size_t i = 1; i < n; i++) {
                auto lcp = lcpsBegin[i];
                set(lcp.lcp-minLCP, lcp.branchingCharacter0);
                set(lcp.lcp-minLCP, lcp.branchingCharacter1);
            }
            max_branching_num = ams[0].size();
            for (size_t i = 1; i < num; i++) {
                if (ams[i].size() > max_branching_num)  max_branching_num = ams[i].size();
            }
            for (adjust_num = num-1; adjust_num >= 0; --adjust_num) {
                if (ams[adjust_num].size()!=0)  break;
            }
            adjust_num += 1;
        }

        /** Returns the rank of a given character in the alphabet if the character is present, otherwise returns 0. */
        size_t rank(size_t n, uint8_t c) const {
            return ams[n].rank(c);
        }

        /** Returns the number of characters in the alphabet. */
        size_t size() const {
            return max_branching_num;
        }

        /** Returns the length of the longest string from this alphabet that can be fit in a 64-bit integer. */
        size_t length64() const {
            return size2length64(size());
        }

        /** Creates a uint64_t from a prefix of the given string. */
        uint64_t readChunk(const char *string, size_t stringLength) const {
            size_t sigma = max_branching_num;
            size_t characters = adjust_num;
            uint64_t chunk = 0;
            size_t i = 0;
            for (; i < characters && i < stringLength; i++)
                chunk = chunk * sigma + std::min<size_t>(rank(i,string[i]), sigma - 1);
            for (; i < characters; i++)
                chunk *= sigma;
            return chunk;
        }

    private:

        size_t size2length64(size_t size) const {
            static const uint8_t lookup[257] = {
                64, 64, 64, 40, 32, 27, 24, 22, 21, 20, 19, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14,
                14, 14, 14, 13, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 11,
                11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 10,
                10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
                10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
                9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                8, 8, 8, 8, 8, 8};
            return lookup[size]; // return size_t(64 / std::log2(std::max<size_t>(2, size)));
            /* Lookup table generation code:
                std::string line;
                for (size_t alphabetSize = 0; alphabetSize <= 256; alphabetSize++) {
                    size_t formula = 64 / std::log2(std::max<size_t>(2, alphabetSize));
                    line += std::to_string(formula) + ",";
                    if (line.length() > 80) {
                        std::cout<<line<<std::endl;
                        line = "";
                    } else {
                        line += " ";
                    }
                }
                std::cout<<line<<std::endl;*/
        }
};
} // namespace lemonhash
