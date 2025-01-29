#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Illegal number of parameters."
    echo "Usage: runExperiments.sh pathToDatasetsFolder pathToCppBinary"
    exit 1
fi

pathToDatasetsFolder=$1
pathToCppBinary=$2

hostname
strings $pathToCppBinary | grep " -m"

# $pathToCppBinary --indexed --withoutAlphabetMaps --fixedEpsilon --numQueries 5M --filename "$pathToDatasetsFolder/dna-31-mer.txt"
# $pathToCppBinary --indexed --withoutAlphabetMaps --fixedEpsilon --numQueries 5M --filename "$pathToDatasetsFolder/trec-text.terms"
# $pathToCppBinary --indexed --withoutAlphabetMaps --fixedEpsilon --numQueries 5M --filename "$pathToDatasetsFolder/uk-2007-05.urls"


########################
# robin does not perform as good as ideal
########################


# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/all_froms.csv" --autotune
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/all_froms.csv" --BS4s
$pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/all_froms.csv" --RSHash4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/all_froms.csv" --stdmap4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/all_froms.csv" --stdumap4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/all_froms.csv" --ART
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/all_froms.csv" --btree4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/all_froms.csv" --robin4s

# $pathToCppBinary --num_keys 1M --numQueries 10M --AN --autotune
# $pathToCppBinary --num_keys 1M --numQueries 10M --AN --BS4s
# $pathToCppBinary --num_keys 1M --numQueries 10M --AN --RSHash4s
# $pathToCppBinary --num_keys 1M --numQueries 10M --AN --stdmap4s
# $pathToCppBinary --num_keys 1M --numQueries 10M --AN --stdumap4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --AN --ART
# $pathToCppBinary --num_keys 1M --numQueries 10M --AN --btree4s
# $pathToCppBinary --num_keys 1M --numQueries 10M --AN --robin4s

# $pathToCppBinary --num_keys 1M --numQueries 10M --Hex --autotune
# $pathToCppBinary --num_keys 1M --numQueries 10M --Hex --BS4s
# $pathToCppBinary --num_keys 1M --numQueries 10M --Hex --RSHash4s
# $pathToCppBinary --num_keys 1M --numQueries 10M --Hex --stdmap4s
# $pathToCppBinary --num_keys 1M --numQueries 10M --Hex --stdumap4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --Hex --ART
# $pathToCppBinary --num_keys 1M --numQueries 10M --Hex --btree4s
# $pathToCppBinary --num_keys 1M --numQueries 10M --Hex --robin4s

# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --autotune
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --BS4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --RSHash4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --stdmap4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --stdumap4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --ART
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --btree4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --robin4s

# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --autotune
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --BS4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --RSHash4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --stdmap4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --stdumap4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --ART
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --btree4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --robin4s
