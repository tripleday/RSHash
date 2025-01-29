cmake_minimum_required(VERSION 3.16)
project(RSHash)

if((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
       AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_compile_options(-march=native)
endif()

find_library(TCMALLOC_LIB tcmalloc)

set(TLX_INSTALL_INCLUDE_DIR tlx CACHE PATH "Workaround for TLX breaking the first cmake call")
add_subdirectory(extlib/tlx EXCLUDE_FROM_ALL)
add_subdirectory(extlib/util EXCLUDE_FROM_ALL)
add_subdirectory(extlib/simpleRibbon EXCLUDE_FROM_ALL)

add_library(PTHash INTERFACE)
target_compile_features(PTHash INTERFACE cxx_std_20)
target_include_directories(PTHash INTERFACE extlib/pthash/include)

option(SUCCINCT_USE_INTRINSICS "Use a set of intrinsics available on all x86-64 architectures" ON)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/extlib/ds2i/succinct/succinct_config.hpp.in
        ${CMAKE_CURRENT_SOURCE_DIR}/extlib/ds2i/succinct/succinct_config.hpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/extlib/ds2i/ds2i_config.hpp.in
        ${CMAKE_CURRENT_SOURCE_DIR}/extlib/ds2i/ds2i_config.hpp ESCAPE_QUOTES)
add_library(ds2i INTERFACE)
target_include_directories(ds2i SYSTEM INTERFACE extlib/ds2i)

add_library(radixspline INTERFACE)
target_compile_features(radixspline INTERFACE cxx_std_20)
target_include_directories(radixspline INTERFACE extlib/RadixSpline/include/rs)

add_library(RSHash INTERFACE)
target_compile_features(RSHash INTERFACE cxx_std_20)
target_include_directories(RSHash INTERFACE include)

target_link_libraries(RSHash INTERFACE SimpleRibbon ByteHamsterUtil ds2i PTHash radixspline ${TCMALLOC_LIB})

# Do not create benchmark targets when this is a subdirectory
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_executable(IntegerDistributions benchmark/integerDistributions.cpp)
    target_compile_features(IntegerDistributions PRIVATE cxx_std_20)
    target_link_libraries(IntegerDistributions PRIVATE RSHash tlx)

    add_executable(StringDistributions benchmark/stringDistributions.cpp)
    target_compile_features(StringDistributions PRIVATE cxx_std_20)
    target_link_libraries(StringDistributions PRIVATE RSHash tlx)
endif()
