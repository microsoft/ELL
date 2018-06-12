////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Convolution.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Convolution.h"
#include "SimpleConvolution.h"
#include "UnrolledConvolution.h"
#include "WinogradConvolution.h"

namespace ell
{
namespace dsp
{
    //
    // Functions
    //

    template<typename ValueType>
    math::RowVector<ValueType> Convolve1D(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, ConvolutionMethodOption method)
    {
        switch (method)
        {
        case ConvolutionMethodOption::automatic:
        // fallthrough
        case ConvolutionMethodOption::simple:
            return Convolve1DSimple(signal, filter);
        case ConvolutionMethodOption::unrolled:
            return Convolve1DUnrolled(signal, filter);
        case ConvolutionMethodOption::winograd:
            return Convolve1DWinograd(signal, filter);
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
    }

    template<typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2D(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, ConvolutionMethodOption method)
    {
        return Convolve2D(signal, filters, numFilters, 1, method);
    }

    template<typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2D(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride, ConvolutionMethodOption method)
    {
        switch (method)
        {
        case ConvolutionMethodOption::automatic:
        // fallthrough
        case ConvolutionMethodOption::simple:
            return Convolve2DSimple(signal, filters, numFilters, stride);
        case ConvolutionMethodOption::unrolled:
            return Convolve2DUnrolled(signal, filters, numFilters, stride);
        case ConvolutionMethodOption::winograd:
            if (stride == 1)
            {
                const int tileSize = 2;
                return Convolve2DWinograd(signal, filters, numFilters, tileSize);
            }
        default:
            break;
        }
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template<typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, ConvolutionMethodOption method)
    {
        return Convolve2DDepthwiseSeparable(signal, filters, numFilters, 1, method);
    }

    template<typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride, ConvolutionMethodOption method)
    {
        switch (method)
        {
        case ConvolutionMethodOption::automatic:
        // fallthrough
        case ConvolutionMethodOption::simple:
            return Convolve2DSimpleDepthwiseSeparable(signal, filters, numFilters, stride);
        case ConvolutionMethodOption::unrolled:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        case ConvolutionMethodOption::winograd:
            if (stride == 1)
            {
                const int tileSize = 2;
                return Convolve2DWinogradDepthwiseSeparable(signal, filters, numFilters, tileSize);
            }
        default:
            break;
        }
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    //
    // Explicit instantiation definitions:
    //
    template math::RowVector<float> Convolve1D(const math::RowVector<float>& signal, const math::RowVector<float>& filter, ConvolutionMethodOption method);
    template math::RowVector<double> Convolve1D(const math::RowVector<double>& signal, const math::RowVector<double>& filter, ConvolutionMethodOption method);

    template math::ChannelColumnRowTensor<float> Convolve2D(const math::ConstChannelColumnRowTensorReference<float>& signal, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, ConvolutionMethodOption method);
    template math::ChannelColumnRowTensor<double> Convolve2D(const math::ConstChannelColumnRowTensorReference<double>& signal, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, ConvolutionMethodOption method);

    template math::ChannelColumnRowTensor<float> Convolve2D(const math::ConstChannelColumnRowTensorReference<float>& signal, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, int stride, ConvolutionMethodOption method);
    template math::ChannelColumnRowTensor<double> Convolve2D(const math::ConstChannelColumnRowTensorReference<double>& signal, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, int stride, ConvolutionMethodOption method);

    template math::ChannelColumnRowTensor<float> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<float>& input, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, ConvolutionMethodOption method);
    template math::ChannelColumnRowTensor<double> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<double>& input, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, ConvolutionMethodOption method);

    template math::ChannelColumnRowTensor<float> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<float>& input, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, int stride, ConvolutionMethodOption method);
    template math::ChannelColumnRowTensor<double> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<double>& input, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, int stride, ConvolutionMethodOption method);
}
}
