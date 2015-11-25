// types.h

#pragma once

#include <cstdint>

// abbreviations for basic types
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

// An entry in a vector
struct indexValue
{
    uint64 index;
    double value;
};

// parent class for all iterators whose Get() function returns an indexValue
struct IIndexValueIterator {};

