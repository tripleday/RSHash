#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>
#include <MultiRetrievalDataStructure.hpp>
#include "support/sequence/BucketOffsets.hpp"

#include <radix_spline.h>
#include <builder.h>

namespace rshash {
/**
 * Learned Monotone Minimal Perfect Hash Function (MMPHF).
 * Each object is mapped to a bucket using Radix Spline.
 * Within the spline, a retrieval data structure explicitly stores the ranks of all objects.
 */
template <size_t retrievalCoeffBits = 64>
class RSHash {
    public:
        size_t N;
    private:
        rs::RadixSpline<uint64_t> rs;
        MultiRetrievalDataStructure<retrievalCoeffBits> retrievalDataStructure;
        lemonhash::BucketOffsets bucketOffsets;
    public:
        static std::string name() {
            return std::string("RSHash")
                   + " retrievalCoeffBits=" + std::to_string(retrievalCoeffBits);
        }

        explicit RSHash(const std::vector<uint64_t> &data)
                : N(data.size()) {
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
            lemonhash::BucketOffsets temp = lemonhash::BucketOffsets(counts.size(), data.size());
            bucketOffsets = std::move(temp);
            for (auto c : counts) {
                bucketOffsets.push(c);
            }
            bucketOffsets.done();
            // auto count = std::count_if(counts.begin(), counts.end(),
            //                                 [](uint64_t n) { return n > 1; });
            // std::cout << "\ncount: " << count << std::endl;
            // auto max_iter = std::max_element(counts.begin(), counts.end());
            // std::cout << "max count: " << *max_iter << std::endl;
        }

        size_t operator()(const uint64_t key) {
            // std::cout << "\nkey:            " << key << std::endl;
            auto [bucketOffset, bucketSize] = bucketOffsets.at((size_t) rs.GetEstimatedPosition(key));
            // std::cout << "bucket index:   " << (size_t) rs.GetEstimatedPosition(key) << std::endl;
            // std::cout << "Bucket offset:  " << bucketOffset << std::endl;
            // std::cout << "Bucket size:    " << bucketSize << std::endl;
            if (bucketSize <= 1) {
                // if (bucketSize==0) {
                //     std::cout << "bucket size is zero" << std::endl;
                // }
                return bucketOffset;
            } else {
                // std::cout << "Retrival query: " << retrievalDataStructure.query(bucketSize, key) << std::endl;
                return bucketOffset + retrievalDataStructure.query(bucketSize, key);
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
