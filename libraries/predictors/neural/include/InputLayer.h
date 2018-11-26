////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Layer.h"

#include <vector>

namespace ell
{
namespace predictors
{
    namespace neural
    {

        /// <summary> An input layer in a neural network. This is the only layer type that takes input from an external source, and not from the output of another layer.
        /// This must be the first layer in the list of layers that get set on a Neural Predictor.
        /// </summary>
        template <typename ElementType>
        class InputLayer : public Layer<ElementType>
        {
        public:
            using Shape = typename Layer<ElementType>::Shape;
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using VectorType = typename Layer<ElementType>::VectorType;
            using TensorType = typename Layer<ElementType>::TensorType;
            using DataVectorType = typename Layer<ElementType>::DataVectorType;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::NumOutputRowsMinusPadding;
            using Layer<ElementType>::NumOutputColumnsMinusPadding;
            using Layer<ElementType>::NumOutputChannels;
            using Layer<ElementType>::AssignValues;

            /// <summary> Parameters common to all layers. </summary>
            struct InputParameters
            {
                /// <summary> Shape of the input tensor. </summary>
                Shape inputShape;

                /// <summary> The padding requirements for the input. </summary>
                PaddingParameters inputPaddingParameters;

                /// <summary> The extents of the tensor in logical order (row, column, channel). This size includes padding. </summary>
                Shape outputShape;

                /// <summary> The padding requirements for the output. </summary>
                PaddingParameters outputPaddingParameters;

                /// <summary> The scale factor to apply to each input value. Default is undefined. </summary>
                ElementType scale;
            };

            /// <summary> Instantiates an instance of an input layer. </summary>
            ///
            /// <param name="inputParameters"> The parameters for the input layer. </param>
            /// <param name="inputParameters">   </param>
            InputLayer(const InputParameters& inputParameters);

            /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
            InputLayer() :
                _data(0, 0, 0) {}

            /// <summary> Sets the input. </summary>
            ///
            /// <param name="input"> Copies the input vector to the input tensor. </param>
            void SetInput(const DataVectorType& input);

            /// <summary> Sets the input. </summary>
            ///
            /// <param name="input"> Copies the input vector to the input tensor. </param>
            void SetInput(const std::vector<ElementType>& input);

            /// <summary> Gets a writeable reference to the input. </summary>
            ///
            /// <returns> The output tensor. </returns>
            TensorType& GetInput() { return _data; }

            /// <summary> Gets a const reference to the input. </summary>
            ///
            /// <returns> The output tensor. </returns>
            const TensorType& GetInput() const { return _data; }

            /// <summary> Feeds the input forward through the layer. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::input; }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("InputLayer"); }

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

            VectorType _scale;
            TensorType _data;
        };

    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

namespace ell
{
namespace predictors
{
    namespace neural
    {

        template <typename ElementType>
        InputLayer<ElementType>::InputLayer(const InputParameters& inputParameters) :
            Layer<ElementType>(LayerParameters{ TensorType(1, 1, 1), inputParameters.inputPaddingParameters, inputParameters.outputShape, inputParameters.outputPaddingParameters }),
            _scale(NumOutputChannels()),
            _data(inputParameters.inputShape)
        {
            _layerParameters.input = _data;
            _scale.Fill(inputParameters.scale);
        }

        template <typename ElementType>
        void InputLayer<ElementType>::SetInput(const DataVectorType& input)
        {
            size_t index = 0;
            auto& inputTensor = _data;

            for (size_t i = 0; i < inputTensor.NumRows(); ++i)
            {
                for (size_t j = 0; j < inputTensor.NumColumns(); ++j)
                {
                    for (size_t k = 0; k < inputTensor.NumChannels(); ++k)
                    {
                        inputTensor(i, j, k) = static_cast<ElementType>(input[index++]);
                    }
                }
            }
        }

        template <typename ElementType>
        void InputLayer<ElementType>::SetInput(const std::vector<ElementType>& input)
        {
            size_t index = 0;
            auto& inputTensor = _data;

            for (size_t i = 0; i < inputTensor.NumRows(); ++i)
            {
                for (size_t j = 0; j < inputTensor.NumColumns(); ++j)
                {
                    for (size_t k = 0; k < inputTensor.NumChannels(); ++k)
                    {
                        inputTensor(i, j, k) = static_cast<ElementType>(input[index++]);
                    }
                }
            }
        }

        template <typename ElementType>
        void InputLayer<ElementType>::Compute()
        {
            auto output = GetOutputMinusPadding();
            auto& input = _layerParameters.input;

            AssignValues(input, output);
            math::ScaleUpdate<math::Dimension::channel>(_scale, output);
        }

        template <typename ElementType>
        void InputLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Layer<ElementType>::WriteToArchive(archiver);

            math::TensorArchiver::Write(_data, "data", archiver);
            if (_scale.Size() > 0)
                archiver["scale"] << _scale[0];
            else
                archiver["scale"] << 1;
        }

        template <typename ElementType>
        void InputLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            math::TensorArchiver::Read(_data, "data", archiver);
            ElementType scale = 1;
            archiver["scale"] >> scale;
            _scale.Resize(NumOutputChannels());
            _scale.Fill(scale);

            _layerParameters.input = _data;
        }

    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
