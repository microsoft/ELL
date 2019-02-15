////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlVectorUtil.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <vector>

namespace ell
{
namespace utilities
{
    template <typename ForwardIterator, typename UnaryOperation>
    auto TransformVector(ForwardIterator begin, ForwardIterator end, UnaryOperation op)
    {
        using ResultType = decltype(op(*begin));
        auto size = std::distance(begin, end);
        std::vector<ResultType> result;
        result.reserve(size);
        std::transform(begin, end, std::back_inserter(result), op);
        return result;
    }
} // namespace utilities
} // namespace ell
