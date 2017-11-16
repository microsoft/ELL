////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PoolingLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <algorithm>

namespace ell
{
namespace predictors
{
namespace neural
{

    template <typename ElementType, template <typename> class PoolingFunctionType>
    PoolingLayer<ElementType, PoolingFunctionType>::PoolingLayer(const LayerParameters& layerParameters, PoolingParameters poolingParameters) :
        Layer<ElementType>(layerParameters),
        _poolingParameters(poolingParameters)
    {
    }

    template <typename ElementType, template <typename> class PoolingFunctionType>
    void PoolingLayer<ElementType, PoolingFunctionType>::Compute()
    {
        auto input = GetInput();
        auto output = GetOutputMinusPadding();
        const size_t poolingWindowSize = _poolingParameters.poolingSize;

        for (size_t row = 0; row < output.NumRows(); row++)
        {
            const size_t startRow = row * _poolingParameters.stride;
            for (size_t column = 0; column < output.NumColumns(); column++)
            {
                const size_t startColumn = column * _poolingParameters.stride;
                std::vector<PoolingFunctionType<ElementType>> poolingValues(output.NumChannels());

                for (size_t pool_y = 0; pool_y < poolingWindowSize; pool_y++)
                {
                    for (size_t pool_x = 0; pool_x < poolingWindowSize; pool_x++)
                    {
                        for (size_t channel = 0; channel < output.NumChannels(); channel++)
                        {
                            // Account for when part of the pooling window falls beyond the pooling region.
                            size_t inputRow = startRow + pool_y;
                            size_t inputColumn = startColumn + pool_x;

                            if ((inputRow < input.NumRows()) && (inputColumn < input.NumColumns()))
                            {
                                poolingValues[channel].Accumulate(input(inputRow, inputColumn, channel));
                            }
                            else
                            {
                                poolingValues[channel].Accumulate(poolingValues[channel].GetValueAtPadding());
                            }
                        }
                    }
                }

                for (size_t channel = 0; channel < output.NumChannels(); channel++)
                {
                    output(row, column, channel) = poolingValues[channel].GetValue();
                }
            }
        }
    }

    template <typename ElementType, template <typename> class PoolingFunctionType>
    bool PoolingLayer<ElementType, PoolingFunctionType>::UsesPadding() const
    {
        const size_t inputDataPaddingSize = GetLayerParameters().inputPaddingParameters.paddingSize;
        const auto inputShape = GetInputShapeMinusPadding();
        const auto outputShape = GetOutputShapeMinusPadding();
        const auto inputWidth = inputShape.NumRows();
        const auto outputWidth = outputShape.NumRows();
        const auto stride = _poolingParameters.stride;
        const auto poolingSize = _poolingParameters.poolingSize;

        const auto paddedOutputWidth = (inputWidth-1) / stride + 1; // ceil(inputWidth/stride);
        const auto nonPaddedOutputWidth = (inputWidth-poolingSize) / stride + 1; // ceil((inputWidth-windowWidth+1) / stride)

        if(outputWidth == nonPaddedOutputWidth)
        {
            return false;
        }
        else if(outputWidth == paddedOutputWidth)
        {
            return true;
        }
        else
        {
            return inputDataPaddingSize != 0;
        }
    }

    template <typename ElementType, template <typename> class PoolingFunctionType>
    void PoolingLayer<ElementType, PoolingFunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Layer<ElementType>::WriteToArchive(archiver);

        archiver["poolingSize"] << _poolingParameters.poolingSize;
        archiver["stride"] << _poolingParameters.stride;
    }

    template <typename ElementType, template <typename> class PoolingFunctionType>
    void PoolingLayer<ElementType, PoolingFunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Layer<ElementType>::ReadFromArchive(archiver);

        archiver["poolingSize"] >> _poolingParameters.poolingSize;
        archiver["stride"] >> _poolingParameters.stride;
    }

}
}
}
