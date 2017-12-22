////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     math_profile_main.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "math_profile.h"
#include "Vector_test.h"
#include "Matrix_test.h"
#include "Tensor_test.h"

using namespace ell;

template<typename ElementType>
void RunProfile()
{
    const size_t repetitions = 10;

    ProfileVectorScaleAdd<ElementType>(100, 100 * repetitions);
    ProfileVectorScaleAdd<ElementType>(10000, 100 * repetitions);
    ProfileVectorScaleAdd<ElementType>(1000000, 100 * repetitions);

    ProfileVectorInner<ElementType>(100, 100 * repetitions);
    ProfileVectorInner<ElementType>(10000, 10 * repetitions);
    ProfileVectorInner<ElementType>(1000000, repetitions);

    constexpr auto column = math::MatrixLayout::columnMajor;
    constexpr auto row = math::MatrixLayout::rowMajor;

    ProfileVectorOuter<ElementType, row>(10, 100 * repetitions);
    ProfileVectorOuter<ElementType, row>(100, 10 * repetitions);
    ProfileVectorOuter<ElementType, row>(1000, repetitions);

    ProfileVectorOuter<ElementType, column>(10, 100 * repetitions);
    ProfileVectorOuter<ElementType, column>(100, 10 * repetitions);
    ProfileVectorOuter<ElementType, column>(1000, repetitions);

    ProfileMatrixVectorMultiplyScaleAddUpdate<ElementType, row>(10, 10, 100 * repetitions);
    ProfileMatrixVectorMultiplyScaleAddUpdate<ElementType, row>(100, 100, 10 * repetitions);
    ProfileMatrixVectorMultiplyScaleAddUpdate<ElementType, row>(1000, 1000, repetitions);

    ProfileMatrixMatrixMultiplyScaleAddUpdate<ElementType, row, row>(10, 10, 10, 100 * repetitions);
    ProfileMatrixMatrixMultiplyScaleAddUpdate<ElementType, row, row>(100, 100, 100, 10 * repetitions);
    ProfileMatrixMatrixMultiplyScaleAddUpdate<ElementType, row, row>(1000, 1000, 1000, repetitions);

    ProfileMatrixMatrixMultiplyScaleAddUpdate<ElementType, row, column>(10, 10, 10, 100 * repetitions);
    ProfileMatrixMatrixMultiplyScaleAddUpdate<ElementType, row, column>(100, 100, 100, 10 * repetitions);
    ProfileMatrixMatrixMultiplyScaleAddUpdate<ElementType, row, column>(1000, 1000, 1000, repetitions);
}

int main()
{
    RunProfile<float>();
    RunProfile<double>();

    return 0;
}