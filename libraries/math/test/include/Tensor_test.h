////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor_test.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// math
#include "Tensor.h"

using namespace ell;

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorIndexer();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorSize();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorNumRows();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorNumColumns();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorNumChannels();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGetShape();

template<typename ElementType>
void TestTensorNumSlices();

template<typename ElementType>
void TestTensorNumPrimarySlices();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorIsEqual();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorEqualityOperator();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorInequalityOoperator();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGetConstReference();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGetSubTensor();

template<typename ElementType>
void TestTensorGetSlice();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGetPrimarySlice();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorReferenceAsVector();

template<typename ElementType>
void TestTensorReferenceAsMatrix();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorCopyFrom();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorReset();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorFill();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGenerate();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorTransform();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorPlusEqualsOperator();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorMinusEqualsOperator();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorTimesEqualsOperator();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorDivideEqualsOperator();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorArchiver();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2, math::ImplementationType implementation>
void TestTensorVectorAddUpdate();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2, math::ImplementationType implementation>
void TestTensorVectorMultiply();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2, math::ImplementationType implementation>
void TestTensorVectorScaleAddUpdate();

#include "../tcc/Tensor_test.tcc"
