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
    }
}
}

#include "../tcc/BatchNormalizationLayer.tcc"
