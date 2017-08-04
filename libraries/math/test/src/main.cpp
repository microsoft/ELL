////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "math_profile.h"
#include "Vector_test.h"
#include "Matrix_test.h"
#include "Tensor_test.h"

using namespace ell;

template<typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void RunOrientedVectorImplementationTests()
{
    TestVectorVectorAddImplementation<ElementType, orientation, implementation>();
    TestVectorScalarMultiplyImplementation<ElementType, orientation, implementation>();
    TestVectorVectorDotImplementation<ElementType, orientation, implementation>();
}

template<typename ElementType, math::VectorOrientation orientation>
void RunOrientedVectorTests()
{
    TestVectorEqualityOperator<ElementType, orientation>();
    TestVectorInequalityOperator<ElementType, orientation>();
    TestVectorGetConstReference<ElementType, orientation>();
    TestVectorGetSubVector<ElementType, orientation>();
    TestVectorTranspose<ElementType, orientation>();
    TestVectorCopyFrom<ElementType, orientation>();
    TestVectorReset<ElementType, orientation>();
    TestVectorFill<ElementType, orientation>();
    TestVectorGenerate<ElementType, orientation>();
    TestVectorTransform<ElementType, orientation>();
    TestVectorResize<ElementType, orientation>();
    TestVectorPrint<ElementType, orientation>();
    TestScalarVectorMultiply<ElementType, orientation>();
    TestVectorSquare<ElementType, orientation>();
    TestVectorSqrt<ElementType, orientation>();
    TestVectorAbs<ElementType, orientation>();
    TestVectorScalarAdd<ElementType, orientation>();
    TestVectorPlusEqualsOperator<ElementType, orientation>();
    TestVectorMinusEqualsOperator<ElementType, orientation>();
    TestVectorVectorAdd<ElementType, orientation>();
    TestVectorScalarMultiply<ElementType, orientation>();
    TestVectorTimesEqualsOperator<ElementType, orientation>();
    TestVectorDivideEqualsOperator<ElementType, orientation>();
    TestVectorScalarMultiplyAdd<ElementType, orientation>();
    TestVectorElementwiseMultiply<ElementType, orientation>();
    TestVectorVectorDot<ElementType, orientation>();
    TestVectorArchiver<ElementType, orientation>();

    RunOrientedVectorImplementationTests<ElementType, orientation, math::ImplementationType::native>();
    RunOrientedVectorImplementationTests<ElementType, orientation, math::ImplementationType::openBlas>();
}

template<typename ElementType>
void RunVectorTests()
{
    TestVectorIndexer<ElementType>();
    TestVectorSize<ElementType>();
    TestVectorGetDataPointer<ElementType>();
    TestVectorGetIncrement<ElementType>();
    TestVectorSwap<ElementType>();
    TestVectorNorm0<ElementType>();
    TestVectorNorm1<ElementType>();
    TestVectorNorm2<ElementType>();
    TestVectorNorm2Squared<ElementType>();
    TestVectorToArray<ElementType>();

    RunOrientedVectorTests<ElementType, math::VectorOrientation::row>();
    RunOrientedVectorTests<ElementType, math::VectorOrientation::column>();
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::ImplementationType implementation>
void RunLayoutMatrixMatrixImplementationTests()
{
    TestMatrixGeneralizedMatrixAdd<ElementType, layout1, layout2, implementation>();
    TestMatrixMatrixMultiply<ElementType, layout1, layout2, implementation>();
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void RunLayoutMatrixImplementationTests()
{
    TestMatrixScalarMultiply<ElementType, layout, implementation>();
    TestMatrixVectorMultiply<ElementType, layout, implementation>();
    TestVectorMatrixMultiply<ElementType, layout, implementation>();

    RunLayoutMatrixMatrixImplementationTests<ElementType, layout, layout, implementation>();
    RunLayoutMatrixMatrixImplementationTests<ElementType, layout, math::TransposeMatrixLayout<layout>::value, implementation>();
}


template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2>
void RunMatrixMatrixLayoutTests()
{
    TestMatrixCopyCtor<ElementType, layout1, layout2>();
}

template <typename ElementType, math::MatrixLayout layout>
void RunMatrixLayoutTests()
{
    TestMatrixNumRows<ElementType, layout>();
    TestMatrixNumColumns<ElementType, layout>();
    TestMatrixSize<ElementType, layout>();
    TestMatrixIndexer<ElementType, layout>();
    TestMatrixGetDataPointer<ElementType, layout>();
    TestMatrixGetLayout<ElementType, layout>();
    TestMatrixIsContiguous<ElementType, layout>();
    TestMatrixSwap<ElementType, layout>();
    TestMatrixIsEqual<ElementType, layout>();
    TestMatrixEqualityOperator<ElementType, layout>();
    TestMatrixInequalityOperator<ElementType, layout>();
    TestMatrixGetConstReference<ElementType, layout>();
    TestMatrixGetSubMatrix<ElementType, layout>();
    TestMatrixGetColumn<ElementType, layout>();
    TestMatrixGetRow<ElementType, layout>();
    TestMatrixGetDiagonal<ElementType, layout>();
    TestMatrixTranspose<ElementType, layout>();
    TestMatrixCopyFrom<ElementType, layout>();
    TestMatrixReset<ElementType, layout>();
    TestMatrixFill<ElementType, layout>();
    TestMatrixGenerate<ElementType, layout>();
    TestMatrixTransform<ElementType, layout>();
    TestMatrixPrint<ElementType, layout>();
    TestMatrixScalarAdd<ElementType, layout>();
    TestMatrixPlusEqualsOperator<ElementType, layout>();
    TestMatrixMinusEqualsOperator<ElementType, layout>();
    TestMatrixColumnwiseSum<ElementType, layout>();
    TestMatrixTimesEqualsOperator<ElementType, layout>();
    TestMatrixDivideEqualsOperator<ElementType, layout>();
    TestMatrixScalarMultiplyAdd<ElementType, layout>();
    TestMatrixElementwiseMultiply<ElementType, layout>();
    TestMatrixArchiver<ElementType, layout>();

    RunMatrixMatrixLayoutTests <ElementType, layout, layout>();
    RunMatrixMatrixLayoutTests <ElementType, layout, math::TransposeMatrixLayout<layout>::value>();

    RunLayoutMatrixImplementationTests<ElementType, layout, math::ImplementationType::native>();
    RunLayoutMatrixImplementationTests<ElementType, layout, math::ImplementationType::openBlas>();
}

template<typename ElementType>
void RunMatrixTests()
{
    TestMatrixGetIncrement<ElementType>();
    TestMatrixNumIntervals<ElementType>();
    TestMatrixGetIntervalSize<ElementType>();
    TestMatrixGetRowIncrement<ElementType>();
    TestMatrixGetColumnIncrement<ElementType>();
    TestMatrixToArray<ElementType>();
    TestMatrixGetMajorVector<ElementType>();

    RunMatrixLayoutTests<ElementType, math::MatrixLayout::columnMajor>();
    RunMatrixLayoutTests<ElementType, math::MatrixLayout::rowMajor>();
}

template <typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2, math::ImplementationType implementation>
void RunLayoutTensorImplementationTests()
{
    TestTensorVectorMultiply<ElementType, dimension0, dimension1, dimension2, implementation>();
}

template <typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void RunLayoutTensorTests()
{
    TestTensorIndexer<ElementType, dimension0, dimension1, dimension2>();
    TestTensorSize<ElementType, dimension0, dimension1, dimension2>();
    TestTensorNumRows<ElementType, dimension0, dimension1, dimension2>();
    TestTensorNumColumns<ElementType, dimension0, dimension1, dimension2>();
    TestTensorNumChannels<ElementType, dimension0, dimension1, dimension2>();
    TestTensorGetShape<ElementType, dimension0, dimension1, dimension2>();
    TestTensorIsEqual<ElementType, dimension0, dimension1, dimension2>();
    TestTensorEqualityOperator<ElementType, dimension0, dimension1, dimension2>();
    TestTensorInequalityOoperator<ElementType, dimension0, dimension1, dimension2>();
    TestTensorGetConstReference<ElementType, dimension0, dimension1, dimension2>();
    TestTensorGetSubTensor<ElementType, dimension0, dimension1, dimension2>();
    TestTensorGetPrimarySlice<ElementType, dimension0, dimension1, dimension2>();
    TestTensorReferenceAsVector<ElementType, dimension0, dimension1, dimension2>();
    TestTensorCopyFrom<ElementType, dimension0, dimension1, dimension2>();
    TestTensorReset<ElementType, dimension0, dimension1, dimension2>();
    TestTensorFill<ElementType, dimension0, dimension1, dimension2>();
    TestTensorGenerate<ElementType, dimension0, dimension1, dimension2>();
    TestTensorTransform<ElementType, dimension0, dimension1, dimension2>();
    TestTensorPlusEqualsOperator<ElementType, dimension0, dimension1, dimension2>();
    TestTensorMinusEqualsOperator<ElementType, dimension0, dimension1, dimension2>();
    TestTensorTimesEqualsOperator<ElementType, dimension0, dimension1, dimension2>();
    TestTensorDivideEqualsOperator<ElementType, dimension0, dimension1, dimension2>();
    TestTensorArchiver<ElementType, dimension0, dimension1, dimension2>();
    TestTensorVectorAdd<ElementType, dimension0, dimension1, dimension2>();
    TestTensorVectorMultiplyAdd<ElementType, dimension0, dimension1, dimension2>();

    RunLayoutTensorImplementationTests<ElementType, dimension0, dimension1, dimension2, math::ImplementationType::native>();
    RunLayoutTensorImplementationTests<ElementType, dimension0, dimension1, dimension2, math::ImplementationType::openBlas>();
}


template <typename ElementType>
void RunTensorTests()
{
    TestTensorGetSlice<ElementType>();
    TestTensorReferenceAsMatrix<ElementType>();
    TestTensorGetLayout<ElementType>();
    TestTensorNumSlices<ElementType>();
    TestTensorNumPrimarySlices<ElementType>();

    RunLayoutTensorTests<ElementType, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    RunLayoutTensorTests<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();
}

int main()
{
    RunVectorTests<float>();
    RunVectorTests<double>();

    RunMatrixTests<float>();
    RunMatrixTests<double>();

    RunTensorTests<float>();
    RunTensorTests<double>();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}