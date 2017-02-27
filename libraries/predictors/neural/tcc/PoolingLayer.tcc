////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PoolingLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PoolingLayer.h"

// stl
#include <algorithm>

namespace ell
{
namespace predictors
{
namespace neural
{
    //static size_t GetPoolingOutputWidth();

    template <typename PoolingFunctionType>
    PoolingLayer<PoolingFunctionType>::PoolingLayer(PoolingParameters poolingParameters) :
        _outputWidth(((poolingParameters.width - poolingParameters.poolingSize + 2 * poolingParameters.padding) / poolingParameters.stride) + 1),
        _outputHeight(((poolingParameters.height - poolingParameters.poolingSize + 2 * poolingParameters.padding) / poolingParameters.stride) + 1),
        _poolingParameters(poolingParameters),
        _output(_outputWidth * _outputHeight * poolingParameters.depth)
    {
    }

    template <typename PoolingFunctionType>
    ILayer::LayerVector& PoolingLayer<PoolingFunctionType>::FeedForward(const ILayer::LayerVector& input)
    {
        intptr_t padding_w_offset = -static_cast<intptr_t>(_poolingParameters.padding);
        intptr_t padding_h_offset = -static_cast<intptr_t>(_poolingParameters.padding);

        size_t inputDepthOffset = 0;
        size_t outputDepthOffset = 0;
        for (size_t c = 0; c < _poolingParameters.depth; c++)
        {
            size_t stride_h_offset = 0;
            size_t heightOffset = 0;
            for (size_t h = 0; h < _outputHeight; h++)
            {
                size_t stride_w_offset = 0;
                for (size_t w = 0; w < _outputWidth; w++)
                {
                    PoolingFunctionType _poolingFunction;
                    // Get the value for each element of the pooling window and accumulate it in the
                    // pooling function.
                    for (size_t pool_y = 0; pool_y < _poolingParameters.poolingSize; pool_y++)
                    {
                        for (size_t pool_x = 0; pool_x < _poolingParameters.poolingSize; pool_x++)
                        {
                            intptr_t value_x = padding_w_offset + stride_w_offset + pool_x;
                            intptr_t value_y = padding_h_offset + stride_h_offset + pool_y;

                            // Check whether we have a real value or whether it is a padded value
                            if (value_x >= 0 && value_x < static_cast<intptr_t>(_poolingParameters.width) && value_y >= 0 && value_y < static_cast<intptr_t>(_poolingParameters.height))
                            {
                                intptr_t value_index = inputDepthOffset + (value_y * _poolingParameters.width) + value_x;

                                _poolingFunction.Accumulate(input[value_index]);
                            }
                            else
                            {
                                _poolingFunction.Accumulate(_poolingFunction.GetValueAtPadding());
                            }
                        }
                    }

                    // Set the output value to the pooling function value
                    size_t outputOffset = outputDepthOffset + heightOffset + w;
                    _output[outputOffset] = _poolingFunction.GetValue();

                    stride_w_offset += _poolingParameters.stride;
                }
                stride_h_offset += _poolingParameters.stride;
                heightOffset += (_outputWidth);
            }
            inputDepthOffset += (_poolingParameters.width * _poolingParameters.height);
            outputDepthOffset += (_outputWidth * _outputHeight);
        }

        return _output;
    }

    template <typename PoolingFunctionType>
    size_t PoolingLayer<PoolingFunctionType>::NumInputs() const 
    { 
        return (_poolingParameters.width * _poolingParameters.height * _poolingParameters.depth);
    }

    template <typename PoolingFunctionType>
    size_t PoolingLayer<PoolingFunctionType>::NumOutputs() const 
    { 
        return _output.Size(); 
    }

    template <typename PoolingFunctionType>
    void PoolingLayer<PoolingFunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["outputWidth"] << _outputWidth;
        archiver["outputHeight"] << _outputHeight;
        archiver["poolingParameters.width"] << _poolingParameters.width;
        archiver["poolingParameters.height"] << _poolingParameters.height;
        archiver["poolingParameters.depth"] << _poolingParameters.depth;
        archiver["poolingParameters.poolingSize"] << _poolingParameters.poolingSize;
        archiver["poolingParameters.stride"] << _poolingParameters.stride;
        archiver["poolingParameters.padding"] << _poolingParameters.padding;

        std::vector<double> temp = _output.ToArray();
        archiver["output"] << temp;
    }

    template <typename PoolingFunctionType>
    void PoolingLayer<PoolingFunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["outputWidth"] >> _outputWidth;
        archiver["outputHeight"] >> _outputHeight;
        archiver["poolingParameters.width"] >> _poolingParameters.width;
        archiver["poolingParameters.height"] >> _poolingParameters.height;
        archiver["poolingParameters.depth"] >> _poolingParameters.depth;
        archiver["poolingParameters.poolingSize"] >> _poolingParameters.poolingSize;
        archiver["poolingParameters.stride"] >> _poolingParameters.stride;
        archiver["poolingParameters.padding"] >> _poolingParameters.padding;

        std::vector<double> temp;
        archiver["output"] >> temp;
        _output = temp;
    }

}
}
}
