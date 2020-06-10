////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixMatrixMultiplyImplementation.h (interfaces)
//  Authors:  Mason Remy
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <nodes/include/MatrixMatrixMultiplyImplementation.h>

enum class MatrixMatrixMultiplyImplementation : int
{
    SimpleForLoops = (int)ell::nodes::MatrixMatrixMultiplyImplementation::SimpleForLoops,
    Mlas_Loopnest_Value = (int)ell::nodes::MatrixMatrixMultiplyImplementation::Mlas_Loopnest_Value,
    ImplementationCount = (int)ell::nodes::MatrixMatrixMultiplyImplementation::LAST
};
