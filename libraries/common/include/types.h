// types.h

#pragma once

#include <cstdint>

typedef uint64_t uint64;

struct indexValue
{
    uint64 index;
    double value;
};

struct IIndexValueIterator {};