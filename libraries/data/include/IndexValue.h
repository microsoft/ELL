////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IndexValue.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <type_traits>

namespace emll
{
namespace linear
{
    /// <summary> An entry in a vector </summary>
    struct IndexValue
    {
        uint64_t index;
        double value;
    };

    // parent classes for iterators
    struct IIndexValueIterator
    {
    };
#ifndef SWIG
    template <typename IteratorType>
    using IsIndexValueIterator = typename std::enable_if_t<std::is_base_of<IIndexValueIterator, IteratorType>::value, bool>;
#endif
}
}
