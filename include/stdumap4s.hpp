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
class stdumap4s {
    public:
        size_t N;
    private:
        std::unordered_map<std::string, uint64_t> um;
    public:
        static std::string name() {
            return std::string("stdumap4s");
        }

        explicit stdumap4s(const std::vector<std::string> &strings)
                : N(strings.size()) {
            for (size_t i = 0; i < strings.size(); i++) {
                um.insert({strings[i], i});
            }
        }

        size_t operator()(const std::string &s) {
            return um[s];
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
