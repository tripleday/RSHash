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
class btree4s {
    public:
        size_t N;
    private:
        // tlx::btree_map<uint64_t, uint64_t> bm;
        stx::btree_map<std::string, uint64_t> bm;
    public:
        static std::string name() {
            return std::string("btree4s");
        }

        explicit btree4s(const std::vector<std::string> &strings)
                : N(strings.size()){
            // for (size_t i = 0; i < strings.size(); i++) {
            //     bm.insert2(strings[i], i);
            // }
            std::vector<std::pair<std::string, uint64_t>> kv_pairs;
            for (uint64_t i = 0; i < N; ++i) {
                kv_pairs.emplace_back(std::make_pair(strings[i], i));
            }
            bm.bulk_load(kv_pairs.begin(),kv_pairs.end());
        }

        size_t operator()(const std::string &s) {
            auto it = bm.lower_bound(s);
            if (it == bm.end())
            {
                return -1;
            }else {
                return it->second;
            }
            // auto it = bm.lower_bound(s);
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
