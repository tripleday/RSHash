#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>

#include <map>

namespace stdmapMethods {
/**
 * std::map
 */
class stdmap {
    public:
        size_t N;
    private:
        std::map<uint64_t, uint64_t> m;
    public:
        static std::string name() {
            return std::string("stdmap");
        }

        explicit stdmap(const std::vector<uint64_t> &data)
                : N(data.size()) {
            for (size_t i = 0; i < data.size(); i++) {
                m.insert({data[i], i});
            }
        }

        size_t operator()(const uint64_t key) {
            return m[key];
        }

        // only for keys
        size_t spaceBits(bool print = true) {
            size_t bytes = sizeof(std::map<uint64_t, uint64_t>) + (m.size() * sizeof(uint64_t));
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace stdmapMethods
