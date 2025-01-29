#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>

#include <tsl/robin_map.h>

namespace robinMethods {
/**
 * std::map
 */
class robin {
    public:
        size_t N;
    private:
        tsl::robin_map<uint64_t, uint64_t> rm;
    public:
        static std::string name() {
            return std::string("robin");
        }

        explicit robin(const std::vector<uint64_t> &data)
                : N(data.size()){
            for (size_t i = 0; i < data.size(); i++) {
                rm.insert({data[i], i});
            }
        }

        size_t operator()(const uint64_t key) {
            return rm[key];
        }

        // only for keys
        size_t spaceBits(bool print = true) {
            size_t bytes = 0;
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace robinMethods
