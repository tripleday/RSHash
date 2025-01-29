#include <unordered_set>
#include <tlx/cmdline_parser.hpp>
#include <LeMonHash.hpp>
#include <LeMonHashHeuristic.hpp>
#include <LeMonHashVL.hpp>
#include <BS.hpp>
#include <RS.hpp>
#include <RSHash.hpp>
#include <stdmap.hpp>
#include <robin.hpp>
#include <btree.hpp>
#include <stdumap.hpp>
#include <PGM.hpp>
#include <bucket_mapping/SuccinctPGMBucketMapper.hpp>
#include <cstdint>
#include "simpleMmphfBenchmark.hpp"
#include "BSBenchmark.hpp"
#include "RSBenchmark.hpp"
#include "RSHashBenchmark.hpp"
#include "stdmapBenchmark.hpp"
#include "robinBenchmark.hpp"
#include "btreeBenchmark.hpp"
#include "stdumapBenchmark.hpp"
#include "pgmBenchmark.hpp"

std::vector<uint64_t> loadEthInteger(std::string &filename, size_t maxN) {
    std::ifstream fileIn(filename, std::ios::in | std::ios::binary);
    if (!fileIn) throw std::system_error(errno, std::system_category(), "failed to open " + filename);

    std::vector<uint64_t> inputData;
    uint64_t number;
    size_t size = 0;
    while (fileIn >> number) {
        inputData.push_back(number);
        size += 1;
        if (size >= maxN)  break;
    }
    fileIn.close();
    std::cout<<"Loading ETH file of size "<<size<<std::endl;

    // std::cout<<"Checking if input data is sorted"<<std::endl;
    // for (size_t i = 1; i < inputData.size(); i++) {
    //     if (inputData.at(i) < inputData.at(i - 1)) {
    //         throw std::runtime_error("Not sorted or duplicate key");
    //     }
    // }
    std::cout<<"Sorting and deduplicating"<<std::endl;
    std::sort(inputData.begin(), inputData.end());
    auto last = std::unique(inputData.begin(), inputData.end());
    inputData.erase(last, inputData.end());
    std::cout<<"Loaded "<<inputData.size()<<" integers"<<std::endl;
    // if (inputData.size() >= 10) {
    //     for (int i = 0; i < 10; ++i) {
    //         std::cout << inputData[i] << ' ';
    //     }
    //     std::cout << std::endl;
    // } else {
    //     std::cout << "Fewer than 10 element in inputData." << std::endl;
    // }
    // std::vector<uint64_t> firstTenElements = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100};
    // return firstTenElements;
    return inputData;
}

std::vector<uint64_t> loadInt64File(std::string &filename, size_t maxN) {
    std::ifstream fileIn(filename, std::ios::in | std::ios::binary);
    if (!fileIn) throw std::system_error(errno, std::system_category(), "failed to open " + filename);
    size_t size = 0;
    fileIn.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    size = std::min(size, maxN);
    std::cout<<"Loading input file of size "<<size<<std::endl;
    std::vector<uint64_t> inputData(size);
    fileIn.read(reinterpret_cast<char *>(inputData.data()), size * sizeof(uint64_t));
    fileIn.close();
    // std::cout<<"Checking if input data is sorted"<<std::endl;
    // for (size_t i = 1; i < inputData.size(); i++) {
    //     if (inputData.at(i) < inputData.at(i - 1)) {
    //         throw std::runtime_error("Not sorted or duplicate key");
    //     }
    // }
    std::cout<<"Sorting and deduplicating"<<std::endl;
    std::sort(inputData.begin(), inputData.end());
    auto last = std::unique(inputData.begin(), inputData.end());
    inputData.erase(last, inputData.end());
    std::cout<<"Loaded "<<inputData.size()<<" integers"<<std::endl;
    if (inputData.size() >= 10) {
        for (int i = 0; i < 10; ++i) {
            std::cout << inputData[i] << ' ';
        }
        std::cout << std::endl;
    } else {
        std::cout << "Fewer than 10 element in inputData." << std::endl;
    }
    // std::vector<uint64_t> firstTenElements = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100};
    // return firstTenElements;
    return inputData;
}

std::vector<uint64_t> loadInt32File(std::string &filename, size_t maxN) {
    std::ifstream fileIn(filename, std::ios::in | std::ios::binary);
    if (!fileIn) throw std::system_error(errno, std::system_category(), "failed to open " + filename);
    uint32_t size = 0;
    fileIn.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));
    size = std::min<uint32_t>(size, maxN);
    std::cout<<"Loading input file of size "<<size<<std::endl;
    std::vector<uint32_t> inputData(size);
    fileIn.read(reinterpret_cast<char *>(inputData.data()), size * sizeof(uint32_t));
    fileIn.close();
    std::cout<<"Loaded "<<inputData.size()<<" integers"<<std::endl;
    std::cout<<"Converting to uint64"<<std::endl;
    std::vector<uint64_t> inputDataConverted;
    inputDataConverted.reserve(size);
    for (size_t i = 0; i < inputData.size(); i++) {
        inputDataConverted.push_back(inputData.at(i));
        if (i > 0 && inputData.at(i) <= inputData.at(i - 1)) {
            throw std::runtime_error("Not sorted or duplicate key");
        }
    }
    return inputDataConverted;
}

std::vector<uint64_t> randomUniform(size_t n, uint64_t u) {
    std::vector<uint64_t> dataset;
    dataset.reserve(n);
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<uint64_t> dist(0, u);
    {
        std::unordered_set<uint64_t> keys;
        keys.reserve(n);
        while (dataset.size() < n) {
            auto key = dist(rng);
            if (keys.insert(key).second)
                dataset.push_back(key);
        }
    }
    std::sort(dataset.begin(), dataset.end());
    if (dataset.back() != u)
        dataset.back() = u;
    return dataset;
}

std::vector<uint64_t> randomPareto(size_t n, double shape = 1.1) {
    std::vector<uint64_t> dataset;
    dataset.reserve(n);
    std::mt19937_64 rng(42);
    std::exponential_distribution<double> dist(shape);
    {
        std::unordered_set<uint64_t> keys;
        keys.reserve(n);
        while (dataset.size() < n) {
            auto key = uint64_t(std::exp(dist(rng)));
            if (keys.insert(key).second)
                dataset.push_back(key);
        }
    }
    std::sort(dataset.begin(), dataset.end());
    return dataset;
}

std::vector<uint64_t> randomExponential(size_t n, uint64_t u) {
    std::vector<uint64_t> dataset;
    dataset.reserve(n);
    std::mt19937_64 rng(42);
    double lambda = 0.1;
    std::exponential_distribution<double> dist(lambda);
    {
        std::unordered_set<uint64_t> keys;
        keys.reserve(n);
        while (dataset.size() < n) {
            auto key = std::min(uint64_t(dist(rng)), u);
            if (keys.insert(key).second)
                dataset.push_back(key);
        }
    }
    std::sort(dataset.begin(), dataset.end());
    return dataset;
}

std::vector<uint64_t> randomNormal(size_t n, uint64_t u) {
    std::vector<uint64_t> dataset;
    dataset.reserve(n);
    std::mt19937_64 rng(42);
    std::normal_distribution<double> dist(u/2, u/4);
    {
        std::unordered_set<uint64_t> keys;
        keys.reserve(n);
        while (dataset.size() < n) {
            auto key = std::min(uint64_t(dist(rng)), u);
            if (keys.insert(key).second)
                dataset.push_back(key);
        }
    }
    std::sort(dataset.begin(), dataset.end());
    return dataset;
}

int main(int argc, char** argv) {
    size_t N = std::numeric_limits<size_t>::max();
    std::string filename;
    std::string type = "int64";
    std::string datasetName = "";
    size_t numQueries = 1e6;
    bool linear = false;
    bool heuristicSegmented = false;
    bool succinctPgm = false;
    bool succinctPgmAutoTune = false;
    bool leMonVl = false;
    bool BS = false;
    bool RS = false;
    bool RSHash = false;
    bool stdmap = false;
    bool robin = false;
    bool btree = false;
    bool stdumap = false;
    bool pgm = false;

    tlx::CmdlineParser cmd;
    cmd.add_bytes('n', "num_keys", N, "Number of keys to generate");
    cmd.add_string('t', "type", type, "Type of data to generate (uniform, pareto, exponential, int32, int64, eth)");
    cmd.add_string('f', "filename", filename, "Input data set to load. First 64 bits must be length, then all following words are integers");
    cmd.add_bytes('q', "numQueries", numQueries, "Number of queries to measure");
    cmd.add_flag("linear", linear, "Run with linear bucket mapper");
    cmd.add_flag("heuristicSegmented", heuristicSegmented, "Run with heuristic segmented bucket mapper");
    cmd.add_flag("succinctPgm", succinctPgm, "Run with succinctPgm bucket mapper");
    cmd.add_flag("succinctPgmAutoTune", succinctPgmAutoTune, "Run with succinctPgmAutoTune bucket mapper");
    cmd.add_flag("leMonVl", leMonVl, "Run with leMonVl bucket mapper");
    cmd.add_flag("BS", BS, "Run with binary search");
    cmd.add_flag("RS", RS, "Run with radix spline");
    cmd.add_flag("RSHash", RSHash, "Run with radix spline hash");
    cmd.add_flag("stdmap", stdmap, "Run with std::map");
    cmd.add_flag("robin", robin, "Run with robin_map");
    cmd.add_flag("btree", btree, "Run with stx::btree");
    cmd.add_flag("stdumap", stdumap, "Run with std::unordered_map");
    cmd.add_flag("pgm", pgm, "Run with Piecewise Geometric Model index (PGM-index)");
    if (!cmd.process(argc, argv)) {
        return 1;
    }
    // if (!linear && !heuristicSegmented && !succinctPgm && !leMonVl && !succinctPgmAutoTune) {
    //     succinctPgmAutoTune = true;
    // }

    std::cout<<"Generating input data"<<std::endl;
    std::vector<uint64_t> inputData;
    if (!filename.empty()) {
        if (type == "int32") {
            inputData = loadInt32File(filename, N);
        } else if (type == "int64") {
            inputData = loadInt64File(filename, N);
        } else if (type == "eth") {
            inputData = loadEthInteger(filename, N);
        } else {
            cmd.print_usage();
            return 1;
        }
        size_t positionOfSlash = filename.find_last_of('/');
        datasetName = positionOfSlash == std::string::npos ? filename : filename.substr(positionOfSlash + 1);
    } else if (type == "pareto") {
        inputData = randomPareto(N, 1);
        datasetName = "pareto";
    } else if (type == "uniform") {
        inputData = randomUniform(N, UINT64_MAX - 1);
        datasetName = "uniform";
    }else if (type == "exponential") {
        inputData = randomExponential(N, UINT64_MAX - 1);
        datasetName = "exponential";
    }else if (type == "normal") {
        inputData = randomNormal(N, UINT64_MAX - 1);
        datasetName = "normal";
    } else {
        cmd.print_usage();
        return 1;
    }

    if (linear) {
        simpleMmphfBenchmark<lemonhash::LeMonHash<lemonhash::LinearBucketMapper>>(inputData, datasetName, numQueries);
    }
    if (heuristicSegmented) {
        simpleMmphfBenchmark<lemonhash::LeMonHashHeuristic<512>>(inputData, datasetName, numQueries);
    }
    if (succinctPgmAutoTune) {
        simpleMmphfBenchmark<lemonhash::LeMonHash<lemonhash::SuccinctPGMBucketMapper<0>>>(inputData, datasetName, numQueries);
    }
    if (succinctPgm) {
        simpleMmphfBenchmark<lemonhash::LeMonHash<lemonhash::SuccinctPGMBucketMapper<>>>(inputData, datasetName, numQueries);
    }
    if (leMonVl) {
        std::vector<std::string> inputAsString;
        inputAsString.reserve(inputData.size());
        for (uint64_t x : inputData) {
            uint64_t swapped = __builtin_bswap64(x);
            inputAsString.emplace_back((char*) &swapped, sizeof(uint64_t));
        }
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<>>(inputAsString, datasetName, numQueries);
    }
    if (BS) {
        BSBenchmark<binarySearchMethods::BS>(inputData, datasetName, numQueries);
    }
    if (RS) {
        RSBenchmark<radixSplineMethods::RS>(inputData, datasetName, numQueries);
    }
    if (RSHash) {
        RSHashBenchmark<rshash::RSHash<>>(inputData, datasetName, numQueries);
    }
    if (stdmap) {
        stdmapBenchmark<stdmapMethods::stdmap>(inputData, datasetName, numQueries);
    }
    if (robin) {
        robinBenchmark<robinMethods::robin>(inputData, datasetName, numQueries);
    }
    if (btree) {
        btreeBenchmark<btreeMethods::btree>(inputData, datasetName, numQueries);
    }
    if (stdumap) {
        stdumapBenchmark<stdumapMethods::stdumap>(inputData, datasetName, numQueries);
    }
    if (pgm) {
        pgmBenchmark<pgmMethods::PGM>(inputData, datasetName, numQueries);
    }
    return 0;
}
