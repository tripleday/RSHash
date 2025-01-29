# RSHash

## Introduction

This is a Monotone Minimal Perfect Hash Function (MMPHF) based on the learned index RadixSpline (https://github.com/learnedsystems/RadixSpline). The implementation is based on LeMonHash (https://github.com/ByteHamster/LeMonHash).

## Requirements

1. Ubuntu 20.04 64bit, gcc11
2. libraries including tbb, xxhash, tcmalloc

## Usage

1. bash prepare.sh
2. bash runIntegers.sh data/ build/IntegerDistributions
3. bash runStrings.sh data/ build/StringDistributions