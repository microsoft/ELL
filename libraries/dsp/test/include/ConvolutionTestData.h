////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTestData.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Tensor.h"

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetReferenceFilter();

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetReferenceSignal();

template <typename ValueType>
ell::math::ChannelColumnRowTensor<ValueType> GetReferenceConvolutionResult();
