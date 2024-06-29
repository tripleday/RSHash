#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>

namespace binarySearchMethods {
/**
 * Binary Search
 */
class BS {
    public:
        size_t N;
    private:
        std::vector<uint64_t> dataStorage;
    public:
        static std::string name() {
            return std::string("BS");
        }

        explicit BS(const std::vector<uint64_t> &data)
                : N(data.size()),
                  dataStorage(data){
        }

        size_t operator()(const uint64_t key) {
            auto it = std::lower_bound(dataStorage.begin(), dataStorage.end(), key);
            if (it != dataStorage.end() && *it == key) {
                return std::distance(dataStorage.begin(), it);
            } else {
                return -1;
            }
        }

        size_t spaceBits(bool print = true) {
            size_t bytes = sizeof(std::vector<uint64_t>) + (dataStorage.capacity() * sizeof(uint64_t));
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace binarySearchMethods
