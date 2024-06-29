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

mappersExceptLinear="--heuristicSegmented --succinctPgm --succinctPgmAutoTune --leMonVl"
allMappers="--linear $mappersExceptLinear"

# $pathToCppBinary $allMappers --numQueries 5M --type int32 --filename "$pathToDatasetsFolder/5GRAM_1"
# $pathToCppBinary $mappersExceptLinear --numQueries 5M --filename "$pathToDatasetsFolder/fb_50M_uint64"
# $pathToCppBinary --heuristicSegmented --numQueries 10 --filename "$pathToDatasetsFolder/fb_50M_uint64"
# $pathToCppBinary $allMappers --numQueries 5M --filename "$pathToDatasetsFolder/osm_cellids_800M_uint64"

# $pathToCppBinary $allMappers --numQueries 5M --filename "$pathToDatasetsFolder/uniform_uint64"
# $pathToCppBinary $allMappers --numQueries 5M --filename "$pathToDatasetsFolder/exponential_uint64"
# $pathToCppBinary $mappersExceptLinear --numQueries 5M --filename "$pathToDatasetsFolder/normal_uint64"
# $pathToCppBinary --linear --numQueries 10k --filename "$pathToDatasetsFolder/normal_uint64"



# $pathToCppBinary --succinctPgmAutoTune --numQueries 10M --filename "$pathToDatasetsFolder/books_50M_uint64"
# $pathToCppBinary --BS --numQueries 10M --filename "$pathToDatasetsFolder/books_50M_uint64"
# $pathToCppBinary --RS --numQueries 10M --filename "$pathToDatasetsFolder/books_50M_uint64"
$pathToCppBinary --RSHash --numQueries 10M --filename "$pathToDatasetsFolder/books_50M_uint64"
# $pathToCppBinary --stdmap --numQueries 10M --filename "$pathToDatasetsFolder/books_50M_uint64"
# $pathToCppBinary --stdumap --numQueries 10M --filename "$pathToDatasetsFolder/books_50M_uint64"

# $pathToCppBinary --succinctPgmAutoTune --numQueries 10M --filename "$pathToDatasetsFolder/fb_50M_uint64"
# $pathToCppBinary --BS --numQueries 10M --filename "$pathToDatasetsFolder/fb_50M_uint64"
# $pathToCppBinary --RS --numQueries 10M --filename "$pathToDatasetsFolder/fb_50M_uint64"
$pathToCppBinary --RSHash --numQueries 10M --filename "$pathToDatasetsFolder/fb_50M_uint64"
# $pathToCppBinary --stdmap --numQueries 10M --filename "$pathToDatasetsFolder/fb_50M_uint64"
# $pathToCppBinary --stdumap --numQueries 10M --filename "$pathToDatasetsFolder/fb_50M_uint64"

# $pathToCppBinary --succinctPgmAutoTune --numQueries 10M --num_keys 10M --type uniform
# $pathToCppBinary --BS --numQueries 10M --num_keys 10M --type uniform
# $pathToCppBinary --RS --numQueries 10M --num_keys 10M --type uniform
$pathToCppBinary --RSHash --numQueries 10M --num_keys 10M --type uniform
# $pathToCppBinary --stdmap --numQueries 10M --num_keys 10M --type uniform
# $pathToCppBinary --stdumap --numQueries 10M --num_keys 10M --type uniform

# $pathToCppBinary --succinctPgmAutoTune --numQueries 10M --num_keys 10M --type normal
# $pathToCppBinary --BS --numQueries 10M --num_keys 10M --type normal
# $pathToCppBinary --RS --numQueries 10M --num_keys 10M --type normal
$pathToCppBinary --RSHash --numQueries 10M --num_keys 10M --type normal
# $pathToCppBinary --stdmap --numQueries 10M --num_keys 10M --type normal
# $pathToCppBinary --stdumap --numQueries 10M --num_keys 10M --type normal
