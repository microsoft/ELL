////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Vector_test.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Operations.h"
#include "Vector.h"

using namespace ell;

template <typename ElementType, math::VectorOrientation Orientation>
void TestVector();

template <typename ElementType, math::ImplementationType Implementation>
void TestVectorOperations();

template <typename ElementType>
void TestElementWiseOperations();

template <typename ElementType>
void TestVectorToArray();

#include "../tcc/Vector_test.tcc"