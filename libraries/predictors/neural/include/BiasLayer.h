////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BiasLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Layer.h"

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> A layer in a neural network that applies a bias to the input. </summary>
        template <typename ElementType>
        class BiasLayer : public Layer<ElementType>
        {
        public:
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using VectorType = typename Layer<ElementType>::VectorType;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::NumOutputChannels;
            using Layer<ElementType>::AssignValues;

            /// <summary> Instantiates an instance of a bias layer. </summary>
            ///
            /// <param name="layerParameters"> The parameters common to every layer. </param>
            /// <param name="bias"> The bias values to apply to input values. </param>
            BiasLayer(const LayerParameters& layerParameters, const VectorType& bias);

            /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
            BiasLayer() {}

            /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::bias; }

            /// <summary> Gets the bias. </summary>
            ///
            /// <returns> The bias. </returns>
            VectorType GetBias() const { return _bias; }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("BiasLayer"); }

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

            VectorType _bias;
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
        BiasLayer<ElementType>::BiasLayer(const LayerParameters& layerParameters, const VectorType& bias) :
            Layer<ElementType>(layerParameters),
            _bias(bias)
        {
            if (this->GetInputShape() != this->GetOutputShapeMinusPadding())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, GetRuntimeTypeName() + ": Expected size of input and output tensor (minus padding) to match");
            }
            if (_bias.Size() != NumOutputChannels())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, GetRuntimeTypeName() + ": Number of 'bias' values must equal number of channels in output");
            }
        }

        template <typename ElementType>
        void BiasLayer<ElementType>::Compute()
        {
            auto output = GetOutputMinusPadding();
            auto input = _layerParameters.input;

            AssignValues(input, output);
            math::AddUpdate<math::Dimension::channel>(_bias, output);
        }

        template <typename ElementType>
        void BiasLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Layer<ElementType>::WriteToArchive(archiver);

            math::VectorArchiver::Write(_bias, "bias", archiver);
        }

        template <typename ElementType>
        void BiasLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            math::VectorArchiver::Read(_bias, "bias", archiver);
        }

    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
