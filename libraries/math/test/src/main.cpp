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

template<typename ElementType, math::ImplementationType implementation>
void RunVectorImplementationTests()
{
    TestVectorVectorInner<ElementType, implementation>();
}

template<typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void RunOrientedVectorImplementationTests()
{
    TestVectorAddUpdateScalar<ElementType, orientation, implementation>();
    TestVectorAddUpdateVector<ElementType, orientation, implementation>();
    TestVectorAddSetScalar<ElementType, orientation, implementation>();
    TestVectorAddSetScalarZero<ElementType, orientation, implementation>();
    TestVectorAddSetScalarOne<ElementType, orientation, implementation>();
    TestVectorAddSetVector<ElementType, orientation, implementation>();
    TestVectorScaleUpdate<ElementType, orientation, implementation>();
    TestVectorScaleUpdateZero<ElementType, orientation, implementation>();
    TestVectorScaleUpdateOne<ElementType, orientation, implementation>();
    TestVectorScaleSet<ElementType, orientation, implementation>();
    TestVectorScaleSetZero<ElementType, orientation, implementation>();
    TestVectorScaleSetOne<ElementType, orientation, implementation>();
    TestVectorScaleAddUpdateScalarVectorOne<ElementType, orientation, implementation>();
    TestVectorScaleAddUpdateScalarOnesScalar<ElementType, orientation, implementation>();
    TestVectorScaleAddUpdateOneVectorScalar<ElementType, orientation, implementation>();
    TestVectorScaleAddUpdateScalarVectorScalar<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnes<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnesScalarZero<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnesScalarOne<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnesZeroScalar<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnesZeroOne<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnesOneZero<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnesZeroOne<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnesZeroZero<ElementType, orientation, implementation>();
    TestVectorScaleAddSetOnesOneOne<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVector<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVectorScalarZero<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVectorScalarOne<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVectorZeroScalar<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVectorZeroOne<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVectorOneZero<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVectorZeroOne<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVectorZeroZero<ElementType, orientation, implementation>();
    TestVectorScaleAddSetVectorOneOne<ElementType, orientation, implementation>();
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
    TestVectorPlusEqualsOperator<ElementType, orientation>();
    TestVectorMinusEqualsOperator<ElementType, orientation>();
    TestVectorTimesEqualsOperator<ElementType, orientation>();
    TestVectorDivideEqualsOperator<ElementType, orientation>();
    TestVectorElementwiseMultiplySet<ElementType, orientation>();
    TestVectorVectorDot<ElementType, orientation>();
    TestVectorArchiver<ElementType, orientation>();
    TestVectorCumulativeSumUpdate<ElementType, orientation>();
    TestVectorConsecutiveDifferenceUpdate<ElementType, orientation>();

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

    RunVectorImplementationTests<ElementType, math::ImplementationType::native>();
    RunVectorImplementationTests<ElementType, math::ImplementationType::openBlas>();
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::MatrixLayout layout3, math::ImplementationType implementation>
void RunTripleLayoutMatrixImplementationTests()
{
    TestMatrixScaleAddSetScalarMatrixOne<ElementType, layout1, layout2, layout3, implementation>();
    TestMatrixScaleAddSetOneMatrixScalar<ElementType, layout1, layout2, layout3, implementation>();
    TestMatrixScaleAddSetScalarMatrixScalar<ElementType, layout1, layout2, layout3,  implementation>();
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::ImplementationType implementation>
void RunDoubleLayoutMatrixImplementationTests()
{
    TestMatrixPlusEqualsOperatorMatrix<ElementType, layout1, layout2, implementation>();
    TestMatrixMinusEqualsOperatorMatrix<ElementType, layout1, layout2, implementation>();
    TestMatrixAddUpdateMatrix<ElementType, layout1, layout2, implementation>();
    TestMatrixAddSetScalar<ElementType, layout1, layout2, implementation>();
    TestMatrixAddSetZero<ElementType, layout1, layout2, implementation>();
    TestMatrixAddSetMatrix<ElementType, layout1, layout2, implementation>();
    TestMatrixScaleSet<ElementType, layout1, layout2, implementation>();
    TestMatrixScaleAddUpdateScalarMatrixOne<ElementType, layout1, layout2, implementation>();
    TestMatrixScaleAddUpdateOneMatrixScalar<ElementType, layout1, layout2, implementation>();
    TestMatrixScaleAddUpdateScalarMatrixScalar<ElementType, layout1, layout2, implementation>();
    TestMatrixMatrixMultiplyScaleAddUpdate<ElementType, layout1, layout2, implementation>();

    RunTripleLayoutMatrixImplementationTests<ElementType, layout1, layout2, layout2, implementation>();
    RunTripleLayoutMatrixImplementationTests<ElementType, layout1, layout2, math::TransposeMatrixLayout<layout2>::value, implementation>();
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void RunLayoutMatrixImplementationTests()
{
    TestMatrixScaleUpdate<ElementType, layout, implementation>();
    TestMatrixAddUpdateScalar<ElementType, layout, implementation>();
    TestMatrixAddUpdateZero<ElementType, layout, implementation>();
    TestMatrixScaleAddUpdateScalarOnesMatrix<ElementType, layout, implementation>();
    TestMatrixVectorMultiplyScaleAddUpdate<ElementType, layout, implementation>();
    TestVectorMatrixMultiplyScaleAddUpdate<ElementType, layout, implementation>();
    TestVectorVectorOuter<ElementType, layout, implementation>();

    RunDoubleLayoutMatrixImplementationTests<ElementType, layout, layout, implementation>();
    RunDoubleLayoutMatrixImplementationTests<ElementType, layout, math::TransposeMatrixLayout<layout>::value, implementation>();
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2>
void RunDoubleLayoutMatrixTests()
{
    TestMatrixCopyCtor<ElementType, layout1, layout2>();
}

template <typename ElementType, math::MatrixLayout layout>
void RunLayoutMatrixTests()
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
    TestMatrixPlusEqualsOperatorScalar<ElementType, layout>();
    TestMatrixMinusEqualsOperatorScalar<ElementType, layout>();
    TestMatrixTimesEqualsOperator<ElementType, layout>();
    TestMatrixDivideEqualsOperator<ElementType, layout>();
    TestMatrixRowwiseSum<ElementType, layout>();
    TestMatrixColumnwiseSum<ElementType, layout>();
    TestMatrixElementwiseMultiplySet<ElementType, layout>();
    TestMatrixRowwiseCumulativeSumUpdate<ElementType, layout>();
    TestMatrixColumnwiseCumulativeSumUpdate<ElementType, layout>();
    TestMatrixRowwiseConsecutiveDifferenceUpdate<ElementType, layout>();
    TestMatrixColumnwiseConsecutiveDifferenceUpdate<ElementType, layout>();
    TestMatrixArchiver<ElementType, layout>();

    RunDoubleLayoutMatrixTests <ElementType, layout, layout>();
    RunDoubleLayoutMatrixTests <ElementType, layout, math::TransposeMatrixLayout<layout>::value>();

    RunLayoutMatrixImplementationTests<ElementType, layout, math::ImplementationType::native>();
    RunLayoutMatrixImplementationTests<ElementType, layout, math::ImplementationType::openBlas>();
}

template<typename ElementType>
void RunMatrixTests()
{
    TestMatrixGetIncrement<ElementType>();
    TestMatrixGetMinorSize<ElementType>();
    TestMatrixGetMajorSize<ElementType>();
    TestMatrixGetRowIncrement<ElementType>();
    TestMatrixGetColumnIncrement<ElementType>();
    TestMatrixToArray<ElementType>();
    TestMatrixGetMajorVector<ElementType>();

    RunLayoutMatrixTests<ElementType, math::MatrixLayout::columnMajor>();
    RunLayoutMatrixTests<ElementType, math::MatrixLayout::rowMajor>();
}

template <typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2, math::ImplementationType implementation>
void RunLayoutTensorImplementationTests()
{
    TestTensorVectorMultiply<ElementType, dimension0, dimension1, dimension2, implementation>();
    TestTensorVectorAddUpdate<ElementType, dimension0, dimension1, dimension2, implementation>();
    TestTensorVectorScaleAddUpdate<ElementType, dimension0, dimension1, dimension2, implementation>();
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

    RunLayoutTensorImplementationTests<ElementType, dimension0, dimension1, dimension2, math::ImplementationType::native>();
    RunLayoutTensorImplementationTests<ElementType, dimension0, dimension1, dimension2, math::ImplementationType::openBlas>();
}

template <typename ElementType>
void RunTensorTests()
{
    TestTensorGetSlice<ElementType>();
    TestTensorReferenceAsMatrix<ElementType>();
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