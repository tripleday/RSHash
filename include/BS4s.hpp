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
        size_t minLCP;
        lemonhash::AlphabetMap<true> am;
        std::vector<uint64_t> dataStorage;
    public:
        static std::string name() {
            return std::string("BS4s");
        }

        explicit BS4s(const std::vector<std::string> &strings) {
            N = strings.size();
            if (N == 0) return;
            auto lcps = lemonhash::computeLCPs(strings.begin(), strings.end());
            minLCP = (*std::min_element(lcps.begin()+1, lcps.end(),
                                [](const lemonhash::LcpDetails& x, const lemonhash::LcpDetails& y) {  return x.lcp < y.lcp; })).lcp;
            std::cout << "\nminLCP " << minLCP << std::endl;   
            lemonhash::AlphabetMap<true> tempa(lcps.begin(), lcps.size());
            am.bitMap[0] = tempa.bitMap[0];
            am.bitMap[1] = tempa.bitMap[1];
            std::cout << "am.length64() " << am.length64() << std::endl;   
            for (auto s : strings) {
                uint64_t r = am.readChunk(s.c_str()+minLCP, s.length()-minLCP);
                dataStorage.emplace_back(r);
            }
        }

        size_t operator()(const std::string &s) {
            uint64_t r = am.readChunk(s.c_str()+minLCP, s.length()-minLCP);
            auto it = std::lower_bound(dataStorage.begin(), dataStorage.end(), r);
            if (it != dataStorage.end() && *it == r) {
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
