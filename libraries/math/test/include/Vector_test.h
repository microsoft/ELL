////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector_test.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Operations.h"
#include "Vector.h"

using namespace ell;

template <typename ElementType, math::VectorOrientation orientation>
void TestVector();

template <typename ElementType, math::ImplementationType implementation>
void TestVectorOperations();

template <typename ElementType>
void TestElementWiseOperations();

template <typename ElementType>
void TestVectorToArray();

template<typename ElementType>
void TestElementwiseTransform();

template<typename ElementType>
void TestTransformedVectors();

template<typename ElementType>
void TestVectorArchiver();

#include "../tcc/Vector_test.tcc"