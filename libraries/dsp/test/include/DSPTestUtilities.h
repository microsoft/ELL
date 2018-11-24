////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DSPTestUtilities.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dsp
#include "Convolution.h"

// math
#include "Matrix.h"
#include "Tensor.h"
#include "Vector.h"

// stl
#include <vector>

template <typename ValueType>
std::ostream& operator<<(std::ostream& os, const std::vector<ValueType>& vec);

template <typename ValueType>
std::string GetSizeString(ell::math::ConstRowMatrixReference<ValueType> input);

template <typename ValueType>
std::string GetSizeString(ell::math::ConstChannelColumnRowTensorReference<ValueType> input);

template <typename ValueType>
std::string GetFilterSizeString(ell::math::ConstRowMatrixReference<ValueType> filter);

template <typename ValueType>
std::string GetFilterSizeString(ell::math::ConstChannelColumnRowTensorReference<ValueType> filters);

std::string GetConvAlgName(ell::dsp::ConvolutionMethodOption alg);

// Helper functions to avoid annoying double-to-float errors
template <typename ValueType, typename ValueType2>
ell::math::RowVector<ValueType> MakeVector(std::initializer_list<ValueType2> list);

template <typename ValueType, typename ValueType2>
ell::math::ChannelColumnRowTensor<ValueType> MakeTensor(std::initializer_list<std::initializer_list<ValueType2>> list);

template <typename ValueType, typename ValueType2>
ell::math::ChannelColumnRowTensor<ValueType> MakeTensor(std::initializer_list<std::initializer_list<std::initializer_list<ValueType2>>> list);

//
// Get some "interesting" input signal data.
//
template <typename ValueType>
void FillInputVector(ell::math::RowVectorReference<ValueType> input);

template <typename ValueType>
void FillInputMatrix(ell::math::RowMatrixReference<ValueType> input);

template <typename ValueType>
void FillInputTensor(ell::math::ChannelColumnRowTensorReference<ValueType> input);

//
// Get some "interesting" filter weights.
//
template <typename ValueType>
void FillFilterVector(ell::math::RowVectorReference<ValueType> filter);

template <typename ValueType>
void FillFilterMatrix(ell::math::RowMatrixReference<ValueType> filter);

template <typename ValueType>
void FillFiltersTensor(ell::math::ChannelColumnRowTensorReference<ValueType> filters, size_t numFilters);
