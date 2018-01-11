////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RecurrentLayer.h (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "TanhActivation.h"

// math
#include "Matrix.h"

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> A layer in a neural network that implements a recurrent layer, meaning that over time the network will recognise temporal behaviour. </summary>
        template <typename ElementType, template <typename> class ActivationFunctionType>
        class RecurrentLayer : public Layer<ElementType>
        {
        public:
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using VectorType = typename Layer<ElementType>::VectorType;
            using MatrixType = typename Layer<ElementType>::MatrixType;
            using MatrixReferenceType = typename Layer<ElementType>::ConstMatrixReferenceType;
            using ConstTensorReferenceType = typename Layer<ElementType>::ConstTensorReferenceType;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::NumOutputRowsMinusPadding;
            using Layer<ElementType>::NumOutputColumnsMinusPadding;
            using Layer<ElementType>::NumOutputChannels;

            /// <summary> Default constructor </summary>
            RecurrentLayer();

            /// <summary> Instantiates an instance of a recurrent layer </summary>
            ///
            /// <param name="layerParameters"> The parameters common to every layer. </param>
            /// <param name="weights"> Weights for the layers should be in the form [Weights, Recurrent weights]. </param>
            /// <param name="biases"> Biases used for computation, should be a vector with the same dimensionality as the output of this layer. </param>
            RecurrentLayer(const LayerParameters& layerParameters, MatrixType& weights, VectorType& biases); //, predictors::neural::ActivationFunctionType activation = predictors::neural::ActivationFunctionType::tanh);

            /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::recurrent; }

            /// <summary> Retrieves the weights applied to the hidden layer </summary>
            ///
            /// <returns> A matrix of weights. </returns>
            const MatrixType& GetHiddenWeights() const { return _hiddenWeights; }

            /// <summary> Retrieves the biases applied to the hidden nodes </summary>
            ///
            /// <returns> A vector of biases. </returns>
            const VectorType& GetHiddenBias() const { return _hiddenBias; }

            /// <summary> Resets the layer's hidden values </summary>
            void Reset();

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType, ActivationFunctionType<ElementType>>("RecurrentLayer"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Gets the activation function. </summary>
            ///
            /// <returns> A const reference to the activation function. </returns>
            const ActivationFunctionType<ElementType>& GetActivationFunction() const { return _activation; }

        protected:
            void WriteToArchive(utilities::Archiver& archiver) const override;
            void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            using Layer<ElementType>::_layerParameters;
            using Layer<ElementType>::_output;

            MatrixType _hiddenWeights;
            VectorType _hiddenBias;

            VectorType _inputPlusHiddenVector;

            ActivationFunctionType<ElementType> _activation;
        };
    }
}
}

#include "../tcc/RecurrentLayer.tcc"
