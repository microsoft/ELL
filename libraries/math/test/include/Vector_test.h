////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector_test.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MatrixOperations.h"
#include "Matrix.h"
#include "Vector.h"

using namespace ell;

template <typename ElementType>
void TestVectorIndexer();

template <typename ElementType>
void TestVectorSize();

template <typename ElementType>
void TestVectorGetDataPointer();

template <typename ElementType>
void TestVectorGetIncrement();

template <typename ElementType>
void TestVectorNorm0();

template <typename ElementType>
void TestVectorNorm1();

template <typename ElementType>
void TestVectorNorm2();

template <typename ElementType>
void TestVectorNorm2Squared();

template <typename ElementType>
void TestVectorToArray();

// ConstVectorReference

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorEqualityOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorInequalityOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorGetConstReference();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorGetSubVector();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorTranspose();

// VectorReference

template <typename ElementType>
void TestVectorSwap();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorCopyFrom();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorReset();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorFill();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorGenerate();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorTransform();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorResize();

// Vector Operations

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorPrint();

template <typename ElementType, math::VectorOrientation orientation>
void TestScalarVectorMultiply();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorSquare();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorSqrt();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorAbs();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorPlusEqualsOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorMinusEqualsOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorTimesEqualsOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorDivideEqualsOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorElementwiseMultiplySet();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorVectorDot();

template <typename ElementType, math::ImplementationType implementation>
void TestVectorVectorInner();

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestVectorVectorOuter();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddUpdateScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddUpdateVector();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddSetScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddSetScalarZero();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddSetScalarOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddSetVector();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleUpdate();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleUpdateZero();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleUpdateOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleSet();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleSetZero();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleSetOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddUpdateScalarVectorOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddUpdateScalarOnesScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddUpdateOneVectorScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddUpdateScalarVectorScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnes();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesScalarZero();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesScalarOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesZeroScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesOneScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesZeroOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesOneZero();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesOneOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesZeroZero();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVector();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorScalarZero();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorScalarOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorZeroScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorOneScalar();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorZeroOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorOneZero();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorOneOne();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorZeroZero();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorCumulativeSumUpdate();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorConsecutiveDifferenceUpdate();

template<typename ElementType>
void TestVectorArchiver();

#include "../tcc/Vector_test.tcc"
