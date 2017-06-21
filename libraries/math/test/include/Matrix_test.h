////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Matrix_test.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix.h"

using namespace ell;

template <typename ElementType, math::MatrixLayout layout>
void TestMatrix1();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrix2();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixArchiver();

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2>
void TestMatrixCopy();

template <typename ElementType>
void TestMatrixReference();

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType Implementation>
void TestMatrixOperations();

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType Implementation>
void TestContiguousMatrixOperations();

template <typename ElementType, math::MatrixLayout layout>
void TestConstMatrixReference();

template <typename ElementType, math::ImplementationType Implementation>
void TestMatrixMatrixAdd();

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType Implementation>
void TestMatrixMatrixMultiply();

#include "../tcc/Matrix_test.tcc"