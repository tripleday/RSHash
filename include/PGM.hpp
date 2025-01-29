#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>
#include "bucket_mapping/support/PGM.hpp"

namespace pgmMethods {
/**
 * Binary Search
 */
class PGM {
    public:
        const uint16_t epsilon = 64; // space-time trade-off parameter
        size_t N;
    private:
        std::vector<uint64_t> dataStorage;
        lemonhash::pgm::PGMIndex<uint64_t> index;
    public:
        static std::string name() {
            return std::string("PGM");
        }

        explicit PGM(const std::vector<uint64_t> &data)
                : N(data.size()),
                  dataStorage(data),
                  index(data.begin(), data.end(), epsilon){
        }

        size_t operator()(const uint64_t key) {
            auto range = index.search(key);
            auto lo = dataStorage.begin() + range.lo;
            auto hi = dataStorage.begin() + range.hi;
            // return *std::lower_bound(lo, hi, key);
            return std::distance(dataStorage.begin(), std::lower_bound(lo, hi, key));
        }

        size_t spaceBits(bool print = true) {
            size_t bytes = index.size_in_bytes() + (dataStorage.capacity() * sizeof(uint64_t));
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace pgmMethods
