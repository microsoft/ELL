////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTestData.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Matrix.h"

template <typename ValueType>
ell::math::RowMatrix<ValueType> GetReferenceMatrixFilter();

template <typename ValueType>
ell::math::RowMatrix<ValueType> GetReferenceMatrixSignal();

template <typename ValueType>
ell::math::RowMatrix<ValueType> GetReferenceMatrixConvolution();
