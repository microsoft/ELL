////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PoolingLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Layer.h"

#include <math/include/Matrix.h>

#include <vector>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Specifies the hyper parameters of the pooling layer. </summary>
        struct PoolingParameters
        {
            /// <summary> Width and height of the pooling field that is slid over the input. </summary>
            size_t poolingSize;

            /// <summary> Number of elements to move/jump when sliding over the input. Often this is the same as poolingSize. </summary>
            size_t stride;
        };

        /// <summary> A layer in a neural network that implements pooling. </summary>
        template <typename ElementType, template <typename> class PoolingFunctionType>
        class PoolingLayer : public Layer<ElementType>
        {
        public:
            using PoolingFunction = PoolingFunctionType<ElementType>;
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using Layer<ElementType>::GetLayerParameters;
            using Layer<ElementType>::GetInput;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::GetInputShapeMinusPadding;
            using Layer<ElementType>::GetOutputShapeMinusPadding;

            /// <summary> Instantiates an instance of a pooling layer. </summary>
            ///
            /// <param name="layerParameters"> The parameters common to every layer. </param>
            /// <param name="poolingParameters"> Specifies the pooling characteristics of the layer. </param>
            PoolingLayer(const LayerParameters& layerParameters, PoolingParameters poolingParameters);

            /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
            PoolingLayer() {}

            /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::pooling; }

            /// <summary> Gets the pooling parameters. </summary>
            ///
            /// <returns> The pooling parameters struct. </returns>
            const PoolingParameters& GetPoolingParameters() const { return _poolingParameters; }

            /// <summary> Indicates if this pooling layer uses padding when computing its output </summary>
            ///
            /// <returns> `true` if using padding (the first window is centered on the first input pixel), `false` if not using padding </returns>
            bool UsesPadding() const;

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType, PoolingFunctionType<ElementType>>("PoolingLayer"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        protected:
            void WriteToArchive(utilities::Archiver& archiver) const override;
            void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            using Layer<ElementType>::_layerParameters;
            using Layer<ElementType>::_output;

            PoolingParameters _poolingParameters;
        };

    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

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

            const auto paddedOutputWidth = (inputWidth - 1) / stride + 1; // ceil(inputWidth/stride);
            const auto nonPaddedOutputWidth = (inputWidth - poolingSize) / stride + 1; // ceil((inputWidth-windowWidth+1) / stride)

            if (outputWidth == nonPaddedOutputWidth)
            {
                return false;
            }
            else if (outputWidth == paddedOutputWidth)
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

    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
