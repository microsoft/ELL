// IndexValue.h

#pragma once

namespace types
{
// An entry in a vector
struct IndexValue
{
    uint64 index;
    double value;
};

// parent classes for iterators 
struct IIndexValueIterator {};


template <typename IteratorType>
using IsIndexValueIterator = typename std::enable_if_t<std::is_base_of<types::IIndexValueIterator, IteratorType>::value, int>;
}
