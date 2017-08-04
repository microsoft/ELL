////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector_test.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MatrixOperations.h"
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
void TestVectorScalarAdd();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorPlusEqualsOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorMinusEqualsOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorVectorAdd();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorVectorAddImplementation();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorScalarMultiply();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScalarMultiplyImplementation();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorTimesEqualsOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorDivideEqualsOperator();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorScalarMultiplyAdd();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorElementwiseMultiply();

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorVectorDot();

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorVectorDotImplementation();

template<typename ElementType>
void TestVectorArchiver();

#include "../tcc/Vector_test.tcc"