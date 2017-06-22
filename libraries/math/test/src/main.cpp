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
    TestElementWiseOperations<float>();

    TestVectorToArray<double>();
    TestVectorToArray<float>();

    TestElementwiseTransform<double>();
    TestElementwiseTransform<float>();

    TestTransformedVectors<double>();
    TestTransformedVectors<float>();

    TestElementwiseTransform<double>();
    TestElementwiseTransform<float>();

    TestTransformedVectors<double>();
    TestTransformedVectors<float>();

    TestVectorArchiver<float, math::VectorOrientation::row>();
    TestVectorArchiver<double, math::VectorOrientation::row>();
    TestVectorArchiver<float, math::VectorOrientation::column>();
    TestVectorArchiver<double, math::VectorOrientation::column>();

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

    TestMatrixArchiver<float, math::MatrixLayout::rowMajor>();
    TestMatrixArchiver<float, math::MatrixLayout::columnMajor>();
    TestMatrixArchiver<double, math::MatrixLayout::rowMajor>();
    TestMatrixArchiver<double, math::MatrixLayout::columnMajor>();

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

    TestContiguousMatrixOperations<float, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestContiguousMatrixOperations<float, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestContiguousMatrixOperations<double, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestContiguousMatrixOperations<double, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestContiguousMatrixOperations<float, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestContiguousMatrixOperations<float, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();
    TestContiguousMatrixOperations<double, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestContiguousMatrixOperations<double, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();

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

    TestCopyFromAndGetSubTensor<double>();
    TestCopyFromAndGetSubTensor<float>();

    TestTensorReference<double>();
    TestTensorReference<float>();

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

    TestTensorVectorAdd<double, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensorVectorAdd<double, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();
    TestTensorVectorAdd<float, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensorVectorAdd<float, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();

    TestTensorVectorMultiply<double, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensorVectorMultiply<double, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();
    TestTensorVectorMultiply<float, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensorVectorMultiply<float, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();

    TestTensorVectorMultiplyAdd<double, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensorVectorMultiplyAdd<double, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();
    TestTensorVectorMultiplyAdd<float, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensorVectorMultiplyAdd<float, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();

    TestTensorArchiver<double, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensorArchiver<double, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();
    TestTensorArchiver<float, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensorArchiver<float, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}