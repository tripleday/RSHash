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
class robin4s {
    public:
        size_t N;
    private:
        tsl::robin_map<std::string, uint64_t> rm;
    public:
        static std::string name() {
            return std::string("robin4s");
        }

        explicit robin4s(const std::vector<std::string> &strings)
                : N(strings.size()) {
            for (size_t i = 0; i < strings.size(); i++) {
                rm.insert({strings[i], i});
            }
        }

        size_t operator()(const std::string &s) {
            return rm[s];
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
