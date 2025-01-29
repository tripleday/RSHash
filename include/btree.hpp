#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>

// #include <extlib/tlx/tlx/container/btree_map.hpp>
#include <stx/btree_map.h>

namespace btreeMethods {
/**
 * std::map
 */
class btree {
    public:
        size_t N;
    private:
        // tlx::btree_map<uint64_t, uint64_t> bm;
        stx::btree_map<uint64_t, uint64_t> bm;
    public:
        static std::string name() {
            return std::string("btree");
        }

        explicit btree(const std::vector<uint64_t> &data)
                : N(data.size()){
            // for (size_t i = 0; i < data.size(); i++) {
            //     bm.insert2(data[i], i);
            // }
            std::vector<std::pair<uint64_t, uint64_t>> kv_pairs;
            for (uint64_t i = 0; i < N; ++i) {
                kv_pairs.emplace_back(std::make_pair(data[i], i));
            }
            bm.bulk_load(kv_pairs.begin(),kv_pairs.end());
        }

        size_t operator()(const uint64_t key) {
            auto it = bm.lower_bound(key);
            if (it == bm.end())
            {
                return -1;
            }else {
                return it->second;
            }
            // auto it = bm.lower_bound(key);
            // if (it != bm.end()) {
            //     return std::distance(bm.begin(), it);
            // } else {
            //     return -1;
            // }
        }

        // only for keys
        size_t spaceBits(bool print = true) {
            size_t bytes = 0;
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace btreeMethods
