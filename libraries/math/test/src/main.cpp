////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector_test.h"
#include "Matrix_test.h"
#include "Tensor_test.h"

using namespace ell;

int main()
{
    //
    // Vector tests
    // 

    TestVector<float, math::VectorOrientation::row>();
    TestVector<double, math::VectorOrientation::row>();
    TestVector<float, math::VectorOrientation::column>();
    TestVector<double, math::VectorOrientation::column>();

    TestVectorOperations<float, math::ImplementationType::native>();
    TestVectorOperations<double, math::ImplementationType::native>();
    TestVectorOperations<float, math::ImplementationType::openBlas>();
    TestVectorOperations<double, math::ImplementationType::openBlas>();

    TestElementWiseOperations<double>();

    TestVectorToArray<double>();
    TestVectorToArray<float>();

    //
    // Matrix tests
    // 

    TestMatrix1<float, math::MatrixLayout::rowMajor>();
    TestMatrix1<float, math::MatrixLayout::columnMajor>();
    TestMatrix1<double, math::MatrixLayout::rowMajor>();
    TestMatrix1<double, math::MatrixLayout::columnMajor>();

    TestMatrix2<float, math::MatrixLayout::rowMajor>();
    TestMatrix2<float, math::MatrixLayout::columnMajor>();
    TestMatrix2<double, math::MatrixLayout::rowMajor>();
    TestMatrix2<double, math::MatrixLayout::columnMajor>();

    TestMatrixReference<int>();

    TestMatrixCopy<float, math::MatrixLayout::rowMajor, math::MatrixLayout::rowMajor>();
    TestMatrixCopy<float, math::MatrixLayout::rowMajor, math::MatrixLayout::columnMajor>();
    TestMatrixCopy<float, math::MatrixLayout::columnMajor, math::MatrixLayout::rowMajor>();
    TestMatrixCopy<float, math::MatrixLayout::columnMajor, math::MatrixLayout::columnMajor>();
    TestMatrixCopy<double, math::MatrixLayout::rowMajor, math::MatrixLayout::rowMajor>();
    TestMatrixCopy<double, math::MatrixLayout::rowMajor, math::MatrixLayout::columnMajor>();
    TestMatrixCopy<double, math::MatrixLayout::columnMajor, math::MatrixLayout::rowMajor>();
    TestMatrixCopy<double, math::MatrixLayout::columnMajor, math::MatrixLayout::columnMajor>();

    TestMatrixOperations<float, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestMatrixOperations<float, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestMatrixOperations<double, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestMatrixOperations<double, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestMatrixOperations<float, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestMatrixOperations<float, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();
    TestMatrixOperations<double, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestMatrixOperations<double, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();

    TestConstMatrixReference<float, math::MatrixLayout::rowMajor>();
    TestConstMatrixReference<float, math::MatrixLayout::rowMajor>();
    TestConstMatrixReference<double, math::MatrixLayout::columnMajor>();
    TestConstMatrixReference<double, math::MatrixLayout::columnMajor>();

    TestMatrixMatrixAdd<float, math::ImplementationType::native>();
    TestMatrixMatrixAdd<float, math::ImplementationType::openBlas>();
    TestMatrixMatrixAdd<double, math::ImplementationType::native>();
    TestMatrixMatrixAdd<double, math::ImplementationType::openBlas>();

    TestMatrixMatrixMultiply<double, math::MatrixLayout::rowMajor, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::rowMajor, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::columnMajor, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::columnMajor, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::rowMajor, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::rowMajor, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::columnMajor, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::columnMajor, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();

    //
    // Tensor tests
    // 

    TestTensor<double, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensor<double, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();
    TestTensor<float, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensor<float, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();

    TestTensorLayout<double>();
    TestTensorLayout<float>();

    TestTensorGetSlice<double>();
    TestTensorGetSlice<float>();
    TestTensorReferenceAsMatrix<double>();
    TestTensorReferenceAsMatrix<float>();

    TestGetSubTensor<double, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestGetSubTensor<double, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();
    TestGetSubTensor<float, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestGetSubTensor<float, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();

    TestGetSubTensorAndReferenceAsMatrix<double>();
    TestGetSubTensorAndReferenceAsMatrix<float>();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}