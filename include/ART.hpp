#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <Function.h>
#include "art/art.hpp"
#include "art/child_it.hpp"
#include "art/inner_node.hpp"
#include "art/leaf_node.hpp"
#include "art/node.hpp"
#include "art/node_16.hpp"
#include "art/node_256.hpp"
#include "art/node_4.hpp"
#include "art/node_48.hpp"
#include "art/tree_it.hpp"


namespace artMethods {
/**
 * Binary Search
 */
class ART {
    public:
        size_t N;
    private:
        // art::art<size_t*> a;
        art::art<size_t> a;
    public:
        static std::string name() {
            return std::string("ART");
        }

        explicit ART(const std::vector<std::string> &strings)
                : N(strings.size()){
            for (size_t i = 0; i < strings.size(); i++) {
                a.set(strings[i].c_str(), i);
                // size_t v = i;
                // a.set(strings[i].c_str(), &v);
                // if (*(a.get(strings[i].c_str())) != i) {
                //     std::cout << "wrong " << strings[i] << std::endl;
                // }
            }
        }

        size_t operator()(const std::string &s) {
            return a.get(s.c_str());
            // if (a.get(s.c_str()) != nullptr) {
            //     return *(a.get(s.c_str()));
            // }
            // else {
            //     return -1;
            // }
        }

        size_t spaceBits(bool print = true) {
            size_t bytes = 0;
            if (print) {
                std::cout << "Average bit per object: " << (8.0 * bytes / N) << std::endl;
            }
            return 8 * bytes;
        }
};
} // namespace artMethods
