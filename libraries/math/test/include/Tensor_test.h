////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
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
void TestTensorGetSlice();

template<typename ElementType>
void TestTensorReferenceAsMatrix();

template<typename ElementType>
void TestGetSubTensorAndReferenceAsMatrix();

#include "../tcc/Tensor_test.tcc"
