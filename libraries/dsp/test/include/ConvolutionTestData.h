////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTestData.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Tensor.h>

template <typename ValueType>
ell::math::RowVector<ValueType> Get1DReferenceFilter();

template <typename ValueType>
ell::math::RowVector<ValueType> Get1DReferenceSignal();

template <typename ValueType>
ell::math::RowVector<ValueType> Get1DReferenceConvolutionResult();

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetReferenceFilter();

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetReferenceSignal();

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetReferenceConvolutionResult();

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetSeparableReferenceFilters();

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetSeparableReferenceSignal();

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetSeparableReferenceConvolutionResult();
