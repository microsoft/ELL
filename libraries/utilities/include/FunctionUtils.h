////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FunctionUtils.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <tuple>

namespace ell
{
namespace utilities
{

    /// <summary> 
    /// InOrderFunctionEvaluator() is a template function that evaluates a number of zero-argument functions in order.
    /// Usage:
    ///
    /// InOrderFunctionEvaluator(f1, f2, f3);
    ///
    /// The above is equivalent to:
    ///
    /// f1(); f2(); f3()
    /// 
    /// </summary>

    /// <summary> Recursive base case with zero functions. Does nothing. </summary>
    inline void InOrderFunctionEvaluator() {}

    /// <summary> Applies a single-argument function to each of a number of arguments. </summary>
    ///
    /// <param name="function"> The first function to evaluate </param>
    /// <param name="functions"> The rest of the functions to evaluate </param>
    template <typename Function, typename... Functions>
    void InOrderFunctionEvaluator(Function&& function, Functions&&... functions);


    /// <summary> 
    /// ApplyToEach() is a template function that applies a single-argument function to each
    /// of a number of arguments.
    /// Usage:
    ///
    /// ApplyToEach(f, arg1, arg2, arg3);
    ///
    /// The above is equivalent to:
    ///
    /// f(arg1); f(arg2); f(arg3);
    /// 
    /// </summary>

    /// <summary> Recursive base case with zero arguments. Does nothing. </summary>
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