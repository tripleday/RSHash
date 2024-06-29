#include <unordered_set>
#include <tlx/cmdline_parser.hpp>
#include <XorShift64.h>
#include <LeMonHashVL.hpp>
#include <LeMonHashVLIndexed.hpp>
#include "simpleMmphfBenchmark.hpp"
#include <RSHash4s.hpp>
#include <RS4s.hpp>
#include <BS4s.hpp>
#include <stdmap4s.hpp>
#include <stdumap4s.hpp>
#include "RSHashBenchmark.hpp"
#include "RSBenchmark.hpp"
#include "BSBenchmark.hpp"
#include "stdmapBenchmark.hpp"
#include "stdumapBenchmark.hpp"

std::string stringOfLength(size_t length, util::XorShift64 &prng) {
    std::string key(length, 'x');
    size_t i = 0;
    while (i < (length & ~0xf)) {
        *((uint64_t*) (key.data() + i)) = prng();
        i += 8;
    }
    while (i < length) {
        key[i] = (char) prng();
        i += 1;
    }
    return key;
}

// alphabet and number string
std::string ANstringOfLength(size_t length, std::mt19937 &prng) {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t charsetSize = sizeof(charset) - 1;
    std::string key;
    key.reserve(length);
    std::uniform_int_distribution<size_t> distribution(0, charsetSize - 1);
    for (size_t i = 0; i < length; ++i) {
        key += charset[distribution(prng)];
    }
    return key;
}

// Hex string
std::string HexstringOfLength(size_t length, std::mt19937 &prng) {
    const char charset[] =
        "0123456789"
        "abcdef";
    const size_t charsetSize = sizeof(charset) - 1;
    std::string key;
    key.reserve(length);
    std::uniform_int_distribution<size_t> distribution(0, charsetSize - 1);
    for (size_t i = 0; i < length; ++i) {
        key += charset[distribution(prng)];
    }
    return key;
}

std::vector<std::string> randomUniformStrings(size_t n, size_t length, size_t commonPrefixLength) {
    std::unordered_set<std::string> keys;
    keys.reserve(n);
    size_t seed = time(nullptr);
    std::cout<<"Seed: "<<seed<<std::endl;
    util::XorShift64 prng(seed);
    std::string prefix = stringOfLength(commonPrefixLength, prng);
    while (keys.size() < n) {
        std::string key = prefix + stringOfLength(length, prng);
        keys.insert(key);
    }
    // keys.insert(std::string("shops"));
    // keys.insert(std::string("shopping"));
    // keys.insert(std::string("shoppers"));
    std::vector<std::string> dataset;
    dataset.insert(dataset.end(), keys.begin(), keys.end());
    std::sort(dataset.begin(), dataset.end());
    for (std::string& str : dataset) {
        std::cout << str << std::endl;
    }
    return dataset;
}

std::vector<std::string> randomUniformANStrings(size_t n, size_t length, size_t commonPrefixLength) {
    std::unordered_set<std::string> keys;
    keys.reserve(n);
    std::random_device rd;
    std::mt19937 prng(rd());
    std::string prefix = ANstringOfLength(commonPrefixLength, prng);
    while (keys.size() < n) {
        std::string key = prefix + ANstringOfLength(length, prng);
        keys.insert(key);
    }
    std::vector<std::string> dataset;
    dataset.insert(dataset.end(), keys.begin(), keys.end());
    std::sort(dataset.begin(), dataset.end());
    // for (std::string& str : dataset) {
    //     std::cout << str << std::endl;
    // }
    return dataset;
}

std::vector<std::string> randomUniformHexStrings(size_t n, size_t length, size_t commonPrefixLength) {
    std::unordered_set<std::string> keys;
    keys.reserve(n);
    std::random_device rd;
    std::mt19937 prng(rd());
    std::string prefix = HexstringOfLength(commonPrefixLength, prng);
    while (keys.size() < n) {
        std::string key = prefix + HexstringOfLength(length, prng);
        keys.insert(key);
    }
    std::vector<std::string> dataset;
    dataset.insert(dataset.end(), keys.begin(), keys.end());
    std::sort(dataset.begin(), dataset.end());
    // for (std::string& str : dataset) {
    //     std::cout << str << std::endl;
    // }
    return dataset;
}

std::vector<std::string> loadFile(std::string &filename, size_t maxStrings) {
    std::vector<std::string> inputData;
    std::ifstream stream(filename);
    if (!stream) throw std::system_error(errno, std::system_category(), "failed to open " + filename);
    const int MAX_LENGTH = 524288;
    char* line = new char[MAX_LENGTH];
    while (stream.getline(line, MAX_LENGTH)) {
        // if (!inputData.empty()) {
        //     if (strcmp(inputData.back().c_str(), line) > 0) {
        //         throw std::runtime_error("Not sorted or duplicate key");
        //     }
        // }
        inputData.emplace_back(line);
        if (inputData.size() >= maxStrings) {
            break;
        }
    }
    delete[] line;
    std::sort(inputData.begin(), inputData.end());
    // for (std::string& str : inputData) {
    //     std::cout << str << std::endl;
    // }
    std::cout<<"Loaded "<<inputData.size()<<" strings"<<std::endl;
    return inputData;
}

int main(int argc, char** argv) {
    size_t N = std::numeric_limits<size_t>::max();
    std::string filename;
    size_t numQueries = 1e6;
    bool autotune = false;
    bool indexed = false;
    bool withoutAlphabetMaps = false;
    bool fixedEpsilon = false;
    bool differentThresholds = false;
    bool AN = false;
    bool Hex = false;
    bool RSHash4s = false;
    bool RS4s = false;
    bool BS4s = false;
    bool stdmap4s = false;
    bool stdumap4s = false;

    tlx::CmdlineParser cmd;
    cmd.add_bytes('n', "num_keys", N, "Number of keys to generate");
    cmd.add_string('f', "filename", filename, "File with input data");
    cmd.add_flag("AN", AN, "Generate random alphabet and numbers");
    cmd.add_flag("Hex", Hex, "Generate random Hexs");
    cmd.add_bytes('q', "numQueries", numQueries, "Number of queries to measure");
    cmd.add_flag("autotune", autotune, "Autotune with alphabet map");
    cmd.add_flag("indexed", indexed, "Include indexed variant");
    cmd.add_flag("withoutAlphabetMaps", withoutAlphabetMaps, "Also run variant without alphabet maps");
    cmd.add_flag("fixedEpsilon", fixedEpsilon, "Also run variant with a fixed epsilon value");
    cmd.add_flag("differentThresholds", differentThresholds, "Also run variant with different thresholds");
    cmd.add_flag("RSHash4s", RSHash4s, "Run with RSHash4s");
    cmd.add_flag("RS4s", RS4s, "Run with RS4s");
    cmd.add_flag("BS4s", BS4s, "Run with BS4s");
    cmd.add_flag("stdmap4s", stdmap4s, "Run with stdmap4s");
    cmd.add_flag("stdumap4s", stdumap4s, "Run with stdumap4s");
    if (!cmd.process(argc, argv)) {
        return 1;
    }

    std::string baseFilename;
    std::vector<std::string> inputData;
    if (filename.empty()) {
        std::cout<<"Generating input data"<<std::endl;
        // inputData = randomUniformStrings(N, 10, 4);
        if (AN) {
            inputData = randomUniformANStrings(N, 10, 4);
            baseFilename = "random_AN";
        }
        if (Hex) {
            inputData = randomUniformHexStrings(N, 10, 4);
            baseFilename = "random_Hex";
        }
        
    } else {
        std::cout<<"Loading file "<<filename<<std::endl;
        inputData = loadFile(filename, N);
        size_t positionOfSlash = filename.find_last_of('/');
        baseFilename = positionOfSlash == std::string::npos ? filename : filename.substr(positionOfSlash + 1);

    }
    if (inputData.empty()) {
        std::cout<<"Empty input data. Used invalid file?"<<std::endl;
        return 1;
    }

    if (autotune) {
    simpleMmphfBenchmark<lemonhash::LeMonHashVL<128, 128, true, lemonhash::UnalignedPGMBucketMapper<0>>>(inputData, baseFilename, numQueries);
    }

    if (withoutAlphabetMaps) {
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<128, 128, false, lemonhash::UnalignedPGMBucketMapper<0>>>(inputData, baseFilename, numQueries);
    }
    if (fixedEpsilon) {
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<128, 128, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
    }
    if (differentThresholds) {
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<64, 32, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<64, 64, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<64, 128, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<64, 256, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);

        simpleMmphfBenchmark<lemonhash::LeMonHashVL<128, 32, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<128, 64, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<128, 128, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<128, 256, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);

        simpleMmphfBenchmark<lemonhash::LeMonHashVL<256, 32, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<256, 64, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<256, 128, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<256, 256, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);

        simpleMmphfBenchmark<lemonhash::LeMonHashVL<512, 32, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<512, 64, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<512, 128, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
        simpleMmphfBenchmark<lemonhash::LeMonHashVL<512, 256, true, lemonhash::UnalignedPGMBucketMapper<63>>>(inputData, baseFilename, numQueries);
    }
    if (indexed) {
        simpleMmphfBenchmark<lemonhash::LeMonHashVLIndexed>(inputData, baseFilename, numQueries);
    }
    if (RSHash4s) {
        RSHashBenchmark<rshash::RSHash4s<>>(inputData, baseFilename, numQueries);
    }
    if (RS4s) {
        RSBenchmark<radixSplineMethods::RS4s>(inputData, baseFilename, numQueries);
    }
    if (BS4s) {
        BSBenchmark<binarySearchMethods::BS4s>(inputData, baseFilename, numQueries);
    }
    if (stdmap4s) {
        stdmapBenchmark<stdmapMethods::stdmap4s>(inputData, baseFilename, numQueries);
    }
    if (stdumap4s) {
        stdumapBenchmark<stdumapMethods::stdumap4s>(inputData, baseFilename, numQueries);
    }

    return 0;
}
