#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>

#include <multi_map.h>

namespace radixSplineMethods {
/**
 * Radix Spline
 */
class RS {
    public:
        size_t N;
    private:
        std::vector<uint64_t> dataStorage;
        rs::RadixSpline<uint64_t> rs;
    public:
        static std::string name() {
            return std::string("RS");
        }

        explicit RS(const std::vector<uint64_t> &data)
                : N(data.size()),
                  dataStorage(data) {
            uint64_t min = data.front();
            uint64_t max = data.back();
            rs::Builder<uint64_t> rsb(min, max, 18, 32);
            for (const auto& key : data) rsb.AddKey(key);
            rs = rsb.Finalize();
        }

        size_t operator()(const uint64_t key) {
            rs::SearchBound bound = rs.GetSearchBound(key);
            auto start = begin(dataStorage) + bound.begin, last = begin(dataStorage) + bound.end;
            return std::lower_bound(start, last, key) - begin(dataStorage);
        }

        size_t spaceBits(bool print = true) {
            size_t bytes = sizeof(std::vector<uint64_t>) + (dataStorage.capacity() * sizeof(uint64_t));
            bytes += rs.GetSize();
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace radixSplineMethods
