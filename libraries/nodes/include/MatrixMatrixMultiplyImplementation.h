////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixMatrixMultiplyImplementation.h (nodes)
//  Authors:  Mason Remy, Denny Sun
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace nodes
{
    enum class MatrixMatrixMultiplyImplementation : int
    {
        SimpleForLoops = 0,
        Mlas_Loopnest_Value,
        LAST,
        DEFAULT = Mlas_Loopnest_Value
    };
} // namespace nodes
} // namespace ell
