////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     FunctionUtils.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <tuple>

namespace emll
{
namespace utilities
{
    inline void InOrderFunctionEvaluator() {}

    template <typename Function, typename... Functions>
    void InOrderFunctionEvaluator(Function&& function, Functions&&... functions);

    template <typename FunctionType>
    inline void ApplyToEach(FunctionType&& function)
    {}

    /// <summary> Applies a single-argument function to each of a number of arguments. </summary>
    ///
    /// <param name="function"> The function to apply </param>
    /// <param name="arg"> The first argument to apply the function to </param>
    /// <param name="args"> The rest of the arguments to apply the function to </param>
    template <typename FunctionType, typename Arg, typename... Args>
    void ApplyToEach(FunctionType&& function, Arg&& arg, Args&&... args);
}
}

#include "../tcc/FunctionUtils.tcc"