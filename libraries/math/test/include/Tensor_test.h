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
void TestTensor();

template<typename ElementType>
void TestTensorReference();

template<typename ElementType>
void TestCopyFromAndGetSubTensor();

template<typename ElementType>
void TestTensorGetSlice();

template<typename ElementType>
void TestTensorReferenceAsMatrix();

template<typename ElementType>
void TestGetSubTensorAndReferenceAsMatrix();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorVectorAdd();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorVectorMultiply();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorVectorMultiplyAdd();

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorArchiver();


#include "../tcc/Tensor_test.tcc"
