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


# $pathToCppBinary --num_keys 10M --numQueries 10M --AN --autotune
# $pathToCppBinary --num_keys 10M --numQueries 10M --AN --BS4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --AN --RS4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --AN --RSHash4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --AN --stdmap4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --AN --stdumap4s

# $pathToCppBinary --num_keys 10M --numQueries 10M --Hex --autotune
# $pathToCppBinary --num_keys 10M --numQueries 10M --Hex --BS4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --Hex --RS4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --Hex --RSHash4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --Hex --stdmap4s
# $pathToCppBinary --num_keys 10M --numQueries 10M --Hex --stdumap4s

# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --autotune
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --BS4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --RS4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --RSHash4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --stdmap4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/dblpxml" --stdumap4s

# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --autotune
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --BS4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --RS4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --RSHash4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --stdmap4s
# $pathToCppBinary --numQueries 10M --filename "$pathToDatasetsFolder/english" --stdumap4s


$pathToCppBinary --num_keys 50K --numQueries 50K --AN --RSHash4s
$pathToCppBinary --num_keys 50K --numQueries 50K --Hex --RSHash4s
