////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     FunctionUtils.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <tuple>
#include <utility> // for integer_sequence

namespace emll
{
namespace utilities
{
    inline void InOrderFunctionEvaluator() {}

    template <typename Function, typename... Functions>
    void InOrderFunctionEvaluator(Function&& function, Functions&&... functions);
}
}

#include "../tcc/FunctionUtils.tcc"