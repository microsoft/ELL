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
void TestMatrixNumRows();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixNumColumns();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixSize();

template <typename ElementType>
void TestMatrixGetIncrement();

template <typename ElementType>
void TestMatrixGetMinorSize();

template <typename ElementType>
void TestMatrixGetMajorSize();

template <typename ElementType>
void TestMatrixGetRowIncrement();

template <typename ElementType>
void TestMatrixGetColumnIncrement();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixIndexer();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetDataPointer();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetLayout();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixIsContiguous();

template <typename ElementType>
void TestMatrixToArray();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixSwap();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixIsEqual();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixEqualityOperator();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixInequalityOperator();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetConstReference();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetSubMatrix();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetColumn();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetRow();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetDiagonal();

template <typename ElementType>
void TestMatrixGetMajorVector();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixTranspose();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixCopyFrom();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixReset();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixFill();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGenerate();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixTransform();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixPrint();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixScalarAdd();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixPlusEqualsOperator();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixMinusEqualsOperator();

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::ImplementationType implementation>
void TestMatrixGeneralizedMatrixAdd();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixRowwiseSum();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixColumnwiseSum();

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestMatrixScalarMultiply();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixTimesEqualsOperator();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixDivideEqualsOperator();

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestMatrixVectorMultiply();

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestVectorMatrixMultiply();

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::ImplementationType implementation>
void TestMatrixMatrixMultiply();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixScalarMultiplyAdd();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixElementwiseMultiply();

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixArchiver();

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2>
void TestMatrixCopyCtor();

#include "../tcc/Matrix_test.tcc"
