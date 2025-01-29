#pragma once

#include <ctime>
#include <chrono>
#include <XorShift64.h>
#include <vector>
#include <iostream>
#include <malloc.h>

template <typename pgms, typename key_t>
void pgmBenchmark(std::vector<key_t> &inputData, std::string &dataset, size_t numQueries) {
    std::cout<<"\r\033[K"<<"Generating "<<pgms::name()<<std::flush;

    // size_t spaceBefore = mallinfo().uordblks;
    size_t spaceBefore;
    MallocExtension::instance()->GetNumericProperty("generic.current_allocated_bytes", &spaceBefore);
    std::chrono::steady_clock::time_point beginConstr = std::chrono::steady_clock::now();
    pgms pgm(inputData);
    std::chrono::steady_clock::time_point endConstr = std::chrono::steady_clock::now();
    // size_t spaceAfter = mallinfo().uordblks;
    size_t spaceAfter;
    MallocExtension::instance()->GetNumericProperty("generic.current_allocated_bytes", &spaceAfter);

    std::cout<<"\r\033[K"<<"Verifying "<<pgms::name()<<std::flush;
    for (size_t i = 0; i < inputData.size()-1; i++) {
        if (pgm(inputData.at(i)) != i) {
            std::cerr<<std::endl<<std::endl<<"Error verifying key "<<i<<std::endl;
        }
        // if (i>=31985430) {
        //     std::cout<<i<<" "<<inputData.at(i)<<std::endl;
        // }
    }

    std::cout<<"\r\033[K"<<"Benchmarking "<<pgms::name()<<std::flush;
    size_t N = inputData.size();
    util::XorShift64 prng(time(nullptr));
    uint64_t h = prng();
    std::chrono::steady_clock::time_point beginQuery = std::chrono::steady_clock::now();
    for (size_t i = 0; i < numQueries; i++) {
        h ^= pgm(inputData.at(h % N)) ^ prng();
    }
    std::chrono::steady_clock::time_point endQuery = std::chrono::steady_clock::now();
    std::cout<<h<<"\r\033[K"<<"Done."<<std::endl;

    double spacePerElement = (double) pgm.spaceBits() / N;

    // This format can be converted to TikZ/PGF plots using https://github.com/bingmann/sqlplot-tools
    std::cout << "RESULT"
              << " dataset=" << dataset
              << " competitor=" << pgms::name()
              << " N=" << N
              << " numQueries=" << numQueries
            //   << " spacePerObject=" << spacePerElement
              << " constructionNs=" << std::chrono::duration_cast<std::chrono::nanoseconds>(endConstr - beginConstr).count()/N
              << " queryNs=" << std::chrono::duration_cast<std::chrono::nanoseconds>(endQuery - beginQuery).count()/numQueries
              << std::endl;
    // Note that this is a bit larger than what is calculated by the data structure itself.
    // The discrepancy decreases the larger we chose N.
    // My guess would be that this is because containers allocate more space than necessary to store their content.
    std::cout<<"Space sanity check: mallinfo returns "<<8.0*(spaceAfter-spaceBefore)/N<<"/object"<<std::endl;
    std::cout << std::endl;
}
