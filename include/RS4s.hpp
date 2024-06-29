#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>
#include "support/sequence/BucketOffsets.hpp"
#include "support/AlphabetMapsCollection.h"

#include <multi_map.h>

namespace radixSplineMethods {
/**
 * Radix Spline
 */
class RS4s {
    public:
        size_t N;
    private:
        size_t minLCP;
        lemonhash::AlphabetMap<true> am;
        std::vector<uint64_t> dataStorage;
        rs::RadixSpline<uint64_t> rs;
    public:
        static std::string name() {
            return std::string("RS4s");
        }

        explicit RS4s(const std::vector<std::string> &strings) {
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
            uint64_t min = dataStorage.front();
            uint64_t max = dataStorage.back();
            rs::Builder<uint64_t> rsb(min, max, 18, 32);
            for (const auto& key : dataStorage) rsb.AddKey(key);
            rs = rsb.Finalize();
        }

        size_t operator()(const std::string &s) {
            uint64_t r = am.readChunk(s.c_str()+minLCP, s.length()-minLCP);
            rs::SearchBound bound = rs.GetSearchBound(r);
            auto start = begin(dataStorage) + bound.begin, last = begin(dataStorage) + bound.end;
            return std::lower_bound(start, last, r) - begin(dataStorage);
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
