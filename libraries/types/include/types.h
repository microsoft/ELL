// types.h

#pragma once

#include <cstdint>
#include <limits>
using std::numeric_limits;
#include <type_traits>

// abbreviations for basic types
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

const uint64 maxUInt64 = numeric_limits<uint64>::max();
const uint32 maxUInt32 = numeric_limits<uint32>::max();
const uint16 maxUInt16 = numeric_limits<uint16>::max();
const uint8 maxUInt8 = numeric_limits<uint8>::max();

// An entry in a vector
struct IndexValue
{
    uint64 index;
    double value;
};

// parent classes for iterators 
struct IIndexValueIterator {};

template <typename IteratorType>
using IsIndexValueIterator = typename std::enable_if_t<std::is_base_of<IIndexValueIterator, IteratorType>::value, int>;
