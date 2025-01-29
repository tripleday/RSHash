#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>
#include "support/AlphabetMapsCollection.h"

#include <map>

namespace stdmapMethods {
/**
 * std::map
 */
class stdmap4s {
    public:
        size_t N;
    private:
        std::map<std::string, uint64_t> m;
    public:
        static std::string name() {
            return std::string("stdmap4s");
        }

        explicit stdmap4s(const std::vector<std::string> &strings)
                : N(strings.size()) {
            for (size_t i = 0; i < strings.size(); i++) {
                m.insert({strings[i], i});
            }
        }

        size_t operator()(const std::string &s) {
            return m[s];
        }

        // only for keys
        size_t spaceBits(bool print = true) {
            size_t bytes = sizeof(std::map<std::string, uint64_t>) + (m.size() * (sizeof(std::string)+sizeof(uint64_t)));
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace stdmapMethods
