#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>
#include "support/AlphabetMapsCollection.h"

#include <multi_map.h>

namespace binarySearchMethods {
/**
 * Binary Search
 */
class BS4s {
    public:
        size_t N;
    private:
        std::vector<std::string> dataStorage;
    public:
        static std::string name() {
            return std::string("BS4s");
        }

        explicit BS4s(const std::vector<std::string> &strings)
                : N(strings.size()),
                  dataStorage(strings){
        }

        size_t operator()(const std::string &s) {
            auto it = std::lower_bound(dataStorage.begin(), dataStorage.end(), s);
            if (it != dataStorage.end() && *it == s) {
                return std::distance(dataStorage.begin(), it);
            } else {
                return -1;
            }
        }

        size_t spaceBits(bool print = true) {
            size_t bytes = sizeof(std::vector<std::string>) + (dataStorage.capacity() * sizeof(std::string));
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace binarySearchMethods
