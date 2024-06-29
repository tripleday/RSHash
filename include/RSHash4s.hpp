#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>
#include <MultiRetrievalDataStructure.hpp>
#include "support/sequence/BucketOffsets.hpp"
#include "support/AlphabetMapsCollection.h"
#include "support/AlphabetMaps.h"

#include <radix_spline.h>
#include <builder.h>

namespace rshash {
/**
 * Learned Monotone Minimal Perfect Hash Function (MMPHF).
 * Each object is mapped to a bucket using Radix Spline.
 * Within the spline, a retrieval data structure explicitly stores the ranks of all objects.
 */
template <size_t retrievalCoeffBits = 64>
class RSHash4s {
    public:
        size_t N;
    private:
        size_t minLCP;
        lemonhash::AlphabetMap<true> am;
        // static const size_t num = 10;
        // lemonhash::AlphabetMaps<true, num> am;
        rs::RadixSpline<uint64_t> rs;
        MultiRetrievalDataStructure<retrievalCoeffBits> retrievalDataStructure;
        lemonhash::BucketOffsets bucketOffsets;
    public:
        static std::string name() {
            return std::string("RSHash4s")
                   + " retrievalCoeffBits=" + std::to_string(retrievalCoeffBits);
        }

        explicit RSHash4s(const std::vector<std::string> &strings) { 
            N = strings.size();
            if (N == 0) return;
            auto lcps = lemonhash::computeLCPs(strings.begin(), strings.end());
            minLCP = (*std::min_element(lcps.begin()+1, lcps.end(),
                                [](const lemonhash::LcpDetails& x, const lemonhash::LcpDetails& y) {  return x.lcp < y.lcp; })).lcp;
            std::cout << "\nminLCP " << minLCP << std::endl;   
            lemonhash::AlphabetMap<true> tempa(lcps.begin(), lcps.size());
            am.bitMap[0] = tempa.bitMap[0];
            am.bitMap[1] = tempa.bitMap[1];
            std::cout << "am.size() " << am.size() << std::endl;  
            std::cout << "am.length64() " << am.length64() << std::endl;  
            // lemonhash::AlphabetMaps<true, num> tempa(lcps.begin(), lcps.size(), minLCP); 
            // for (int i = 0; i < num; ++i){
            //     am.ams[i].bitMap[0] = tempa.ams[i].bitMap[0];
            //     am.ams[i].bitMap[1] = tempa.ams[i].bitMap[1];
            //     std::cout << "am.ams[i].size() " <<  am.ams[i].size() << std::endl; 
            // }
            // am.max_branching_num = tempa.max_branching_num;
            // am.adjust_num = tempa.adjust_num;
            // std::cout << "am.max_branching_num " << am.max_branching_num << std::endl;  
            // std::cout << "am.adjust_num " << am.adjust_num << std::endl;  
            // std::cout << "am.length64() " << am.length64() << std::endl;  
            std::vector<uint64_t> data;
            // data.reserve(N);
            for (auto s : strings) {
                uint64_t r = am.readChunk(s.c_str()+minLCP, s.length()-minLCP);
                data.emplace_back(r);
            }
            uint64_t min = data.front();
            uint64_t max = data.back();
            // std::cout << "\nmax-min " << (max-min) << std::endl;
            // std::cout << "\nmax-min " << std::log2(max-min) << std::endl;
            rs::Builder<uint64_t> rsb(min, max, 10, 256);
            for (const auto& key : data) rsb.AddKey(key);
            rs = rsb.Finalize();
            std::vector<size_t> counts;

            size_t last_p = 0;
            for (size_t i = 0; i < data.size(); ) {
                size_t p = rs.GetEstimatedPosition(data[i]);
                if (i == 0 || (p != last_p)) {
                    size_t bucket_diff = p - last_p;
                    if (bucket_diff > 1) {
                        for (size_t t = 0; t < bucket_diff - 1; t++) {
                            counts.emplace_back(0);
                        }  
                    }
                    size_t count = 1;
                    size_t j = i + 1;
                    for (; j < data.size() && p == (size_t) rs.GetEstimatedPosition(data[j]); j++) {
                        count++;
                    }
                    if (count > 1) {
                        for (size_t k = 0; k < count; k++) {
                            retrievalDataStructure.addInput(count, data[i+k], k);
                        }
                    }
                    counts.emplace_back(count);
                    i = j;
                }
                last_p = p;
            }
            retrievalDataStructure.build();
            lemonhash::BucketOffsets tempb = lemonhash::BucketOffsets(counts.size(), data.size());
            bucketOffsets = std::move(tempb);
            for (auto c : counts) {
                bucketOffsets.push(c);
            }
            bucketOffsets.done();
        }

        size_t operator()(const std::string &s) {
            uint64_t r = am.readChunk(s.c_str()+minLCP, s.length()-minLCP);
            // std::cout << "\nkey:            " << key << std::endl;
            auto [bucketOffset, bucketSize] = bucketOffsets.at((size_t) rs.GetEstimatedPosition(r));
            // std::cout << "bucket index:   " << (size_t) rs.GetEstimatedPosition(key) << std::endl;
            // std::cout << "Bucket offset:  " << bucketOffset << std::endl;
            // std::cout << "Bucket size:    " << bucketSize << std::endl;
            if (bucketSize <= 1) {
                return bucketOffset;
            } else {
                // std::cout << "Retrival query: " << retrievalDataStructure.query(bucketSize, key) << std::endl;
                return bucketOffset + retrievalDataStructure.query(bucketSize, r);
            }
        }

        size_t spaceBits(bool print = true) {
            if (print) {
                std::cout << "Retrieval:      " << (double(retrievalDataStructure.spaceBits(N)) / N) << std::endl;
                std::cout << "Radix Spline: " << (8.0 * rs.GetSize() / N) << std::endl;
                std::cout << "Bucket offsets: " << (8.0 * bucketOffsets.space() / N) << std::endl;
            }
            size_t bytes = sizeof(*this) + rs.GetSize() + bucketOffsets.space();
            return 8 * bytes + retrievalDataStructure.spaceBits(0);
        }
};
} // namespace rshash
