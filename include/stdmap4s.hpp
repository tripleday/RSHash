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
        size_t minLCP;
        lemonhash::AlphabetMap<true> am;
        std::map<uint64_t, uint64_t> m;
    public:
        static std::string name() {
            return std::string("stdmap4s");
        }

        explicit stdmap4s(const std::vector<std::string> &strings) {
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
            std::vector<uint64_t> data;
            for (auto s : strings) {
                uint64_t r = am.readChunk(s.c_str()+minLCP, s.length()-minLCP);
                data.emplace_back(r);
            }
            for (size_t i = 0; i < data.size(); i++) {
                m.insert({data[i], i});
            }
        }

        size_t operator()(const std::string &s) {
            uint64_t r = am.readChunk(s.c_str()+minLCP, s.length()-minLCP);
            return m[r];
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
