#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>

#include <unordered_map>

namespace stdumapMethods {
/**
 * std::map
 */
class stdumap {
    public:
        size_t N;
    private:
        std::unordered_map<uint64_t, uint64_t> um;
    public:
        static std::string name() {
            return std::string("stdumap");
        }

        explicit stdumap(const std::vector<uint64_t> &data)
                : N(data.size()) {
            for (size_t i = 0; i < data.size(); i++) {
                um.insert({data[i], i});
            }
        }

        size_t operator()(const uint64_t key) {
            return um[key];
        }

        // only for keys
        size_t spaceBits(bool print = true) {
            size_t bytes = sizeof(std::unordered_map<uint64_t, uint64_t>) + (um.size() * sizeof(uint64_t));
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace stdumapMethods
