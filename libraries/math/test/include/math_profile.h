////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     math_profile.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix.h"

using namespace ell;

// stl
#include <string>

template<typename ElementType>
void ProfileVectorScaleAdd(size_t size, size_t repetitions, std::string seed = "123ABC");

template <typename ElementType>
void ProfileVectorInner(size_t size, size_t repetitions, std::string seed = "123ABC");

template <typename ElementType, math::MatrixLayout layout>
void ProfileVectorOuter(size_t size, size_t repetitions, std::string seed = "123ABC");

template <typename ElementType, math::MatrixLayout layout>
void ProfileMatrixVectorMultiplyScaleAddUpdate(size_t numRows, size_t numColumns, size_t repetitions, std::string seed = "123ABC");

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2>
void ProfileMatrixMatrixMultiplyScaleAddUpdate(size_t numRows, size_t numColumns, size_t numColumns2, size_t repetitions, std::string seed = "123ABC");

#include "../tcc/math_profile.tcc"
