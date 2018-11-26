////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BatchNormalizationLayer.h (neural)
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
        /// <summary> Indicates what term the epsilon will be added to in the denominator. </summary>
        enum class EpsilonSummand : int
        {
            Variance,
            SqrtVariance
        };

        /// <summary> A layer in a neural network that applies batch normalization to the input. </summary>
        template <typename ElementType>
        class BatchNormalizationLayer : public Layer<ElementType>
        {
        public:
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using VectorType = typename Layer<ElementType>::VectorType;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::NumOutputRowsMinusPadding;
            using Layer<ElementType>::NumOutputColumnsMinusPadding;
            using Layer<ElementType>::NumOutputChannels;
            using Layer<ElementType>::AssignValues;

            /// <summary> Instantiates an instance of a batch normalization layer. </summary>
            ///
            /// <param name="layerParameters"> The parameters common to every layer. </param>
            /// <param name="mean"> The mean values. </param>
            /// <param name="variance"> The variance values. </param>
            /// <param name="epsilon"> The epsilon added to the denominator to avoid division by zero. </param>
            /// <param name="EpsilonSummand"> Which component will the epsilon will be applied to the denominator. </param>
            BatchNormalizationLayer(const LayerParameters& layerParameters, const VectorType& mean, const VectorType& variance, ElementType epsilon, EpsilonSummand epsilonSummand);

            /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
            BatchNormalizationLayer() {}

            /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::batchNormalization; }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("BatchNormalizationLayer"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Returns the value to scale the output by. </summary>
            ///
            /// <returns> The value to scale the output by. </returns>
            const VectorType& GetScale() const { return _multiplicationValues; }

            /// <summary> Returns the value to offset the output by. </summary>
            ///
            /// <returns> The value to offset the output by. </returns>
            const VectorType& GetBias() const { return _additionValues; }

        protected:
            void WriteToArchive(utilities::Archiver& archiver) const override;
            void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            using Layer<ElementType>::_layerParameters;
            using Layer<ElementType>::_output;

            VectorType _multiplicationValues;
            VectorType _additionValues;
            ElementType _epsilon; // To ensure non-zero division, this is added to denominator
            EpsilonSummand _epsilonSummand;
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

#include <cmath>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        BatchNormalizationLayer<ElementType>::BatchNormalizationLayer(const LayerParameters& layerParameters, const VectorType& mean, const VectorType& variance, ElementType epsilon, EpsilonSummand epsilonSummand) :
            Layer<ElementType>(layerParameters),
            _multiplicationValues(mean.Size()),
            _additionValues(variance.Size()),
            _epsilon(epsilon),
            _epsilonSummand(epsilonSummand)
        {
            if (mean.Size() != variance.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, GetRuntimeTypeName() + ": Size of 'mean' and 'variance' must match");
            }
            if (_layerParameters.input.Size() != GetOutputMinusPadding().Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, GetRuntimeTypeName() + ": Expected size of input and output tensor (minus padding) to match");
            }
            if (mean.Size() != NumOutputChannels())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, GetRuntimeTypeName() + ": Number of 'mean' and 'variance' values must equal number of channels in output");
            }

            // Batch norm is: outputValue = (inputValue - mean) / (sqrt(variance) + _epsilon)
            // To turn this into one MultiplyAdd operation, we can rearrange it to:
            // EpsilonSummand::Variance:
            //   outputValue = inputValue * (1/(sqrt(variance + _epsilon))) + (-mean * 1/(sqrt(variance + _epsilon)))
            // EpsilonSummand::SqrtVariance:
            //   outputValue = inputValue * (1/(sqrt(variance) + _epsilon)) + (-mean * 1/(sqrt(variance) + _epsilon))
            for (size_t i = 0; i < _additionValues.Size(); i++)
            {
                ElementType varianceFactor = (_epsilonSummand == EpsilonSummand::Variance) ? (1 / (std::sqrt(variance[i] + _epsilon))) : (1 / (std::sqrt(variance[i]) + _epsilon));

                _multiplicationValues[i] = varianceFactor;
                _additionValues[i] = -mean[i] * varianceFactor;
            }
        }

        template <typename ElementType>
        void BatchNormalizationLayer<ElementType>::Compute()
        {
            auto output = GetOutputMinusPadding();
            auto input = _layerParameters.input;

            AssignValues(input, output);
            math::ScaleAddUpdate<math::Dimension::channel>(_multiplicationValues, _additionValues, output);
        }

        template <typename ElementType>
        void BatchNormalizationLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Layer<ElementType>::WriteToArchive(archiver);

            math::VectorArchiver::Write(_multiplicationValues, "multiplicationValues", archiver);
            math::VectorArchiver::Write(_additionValues, "additionValues", archiver);

            archiver["epsilon"] << _epsilon;
            archiver["epsilonSummand"] << static_cast<int>(_epsilonSummand);
        }

        template <typename ElementType>
        void BatchNormalizationLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            math::VectorArchiver::Read(_multiplicationValues, "multiplicationValues", archiver);
            math::VectorArchiver::Read(_additionValues, "additionValues", archiver);

            archiver["epsilon"] >> _epsilon;

            int value;
            archiver["epsilonSummand"] >> value;
            _epsilonSummand = static_cast<EpsilonSummand>(value);
        }
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
