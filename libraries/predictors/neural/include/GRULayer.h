////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GRULayer.h (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "HardSigmoidActivation.h"
#include "Layer.h"
#include "SigmoidActivation.h"
#include "SoftMaxActivation.h"
#include "TanhActivation.h"

// math
#include "Matrix.h"

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Specifies the parameters of the GRU layer. </summary>
        template <typename ElementType>
        struct GRUParameters
        {
            typename Layer<ElementType>::MatrixType& updateWeights;
            typename Layer<ElementType>::MatrixType& resetWeights;
            typename Layer<ElementType>::MatrixType& hiddenWeights;

            typename Layer<ElementType>::VectorType& updateBias;
            typename Layer<ElementType>::VectorType& resetBias;
            typename Layer<ElementType>::VectorType& hiddenBias;
        };

        /// <summary> 
        /// A layer in a recurrent network that implements an GRU layer, meaning this layer retains gated "memory"
        /// over time and uses this information to inform predictions.
        /// </summary>
        template <typename ElementType, template <typename> class ActivationFunctionType = TanhActivation, template <typename> class RecurrentActivationFunctionType = SigmoidActivation>
        class GRULayer : public Layer<ElementType>
        {
        public:
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using VectorType = typename Layer<ElementType>::VectorType;
            using MatrixType = typename Layer<ElementType>::MatrixType;
            using MatrixReferenceType = typename Layer<ElementType>::MatrixReferenceType;
            using ConstTensorReferenceType = typename Layer<ElementType>::ConstTensorReferenceType;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::NumOutputRowsMinusPadding;
            using Layer<ElementType>::NumOutputColumnsMinusPadding;
            using Layer<ElementType>::NumOutputChannels;

            /// <summary> Default constructor </summary>
            GRULayer();

            /// <summary> Instantiates an instance of a GRU layer. </summary>
            ///
            /// <param name="layerParameters"> The parameters common to every layer. </param>
            /// <param name="parameters"> The weights and biases applicable to an GRU. Weights should be organised as: [weights, recurrent layer weights] or [W, U].
            /// Biases should be compatible in dimensionality with the output of the network.</param>
            GRULayer(const LayerParameters& layerParameters, GRUParameters<ElementType>& parameters);

            /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::gru; }

            /// <summary> Retrieves the weights applied to the update layer </summary>
            ///
            /// <returns> A matrix of weights. </returns>
            const MatrixType& GetUpdateWeights() const { return _updateWeights; }

            /// <summary> Retrieves the weights applied to the reset layer </summary>
            ///
            /// <returns> A matrix of weights. </returns>
            const MatrixType& GetResetWeights() const { return _resetWeights; }

            /// <summary> Retrieves the weights applied to the hidden layer </summary>
            ///
            /// <returns> A matrix of weights. </returns>
            const MatrixType& GetHiddenWeights() const { return _hiddenWeights; }

            /// <summary> Retrieves the biases applied to the update nodes </summary>
            ///
            /// <returns> A vector of biases. </returns>
            const VectorType& GetUpdateBias() const { return _updateBias; }

            /// <summary> Retrieves the biases applied to the reset nodes </summary>
            ///
            /// <returns> A vector of biases. </returns>
            const VectorType& GetResetBias() const { return _resetBias; }

            /// <summary> Retrieves the biases applied to the hidden nodes </summary>
            ///
            /// <returns> A vector of biases. </returns>
            const VectorType& GetHiddenBias() const { return _hiddenBias; }

            /// <summary> Retrieves the enum of the activation function currently in use by this layer </summary>
            ///
            /// <returns> The ActivationFunctionType </returns>
            const ActivationFunctionType<ElementType> GetActivationFunction() const { return _activationFunction; }

            /// <summary> Retrieves the enum of the recurrent activation function currently in use by this layer </summary>
            ///
            /// <returns> The ActivationFunctionType </returns>
            const RecurrentActivationFunctionType<ElementType> GetRecurrentActivationFunction() const { return _recurrentActivationFunction; }

            /// <summary> Resets the layer's hidden values </summary>
            void Reset();

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType, ActivationFunctionType<ElementType>, RecurrentActivationFunctionType<ElementType>>("GRULayer"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        protected:
            virtual void WriteToArchive(utilities::Archiver& archiver) const override;
            virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            using Layer<ElementType>::_layerParameters;
            using Layer<ElementType>::_output;

            MatrixType _updateWeights;
            MatrixType _resetWeights;
            MatrixType _hiddenWeights;

            VectorType _updateBias;
            VectorType _resetBias;
            VectorType _hiddenBias;

            VectorType _inputPlusHiddenVector;

            ActivationFunctionType<ElementType> _activationFunction;
            RecurrentActivationFunctionType<ElementType> _recurrentActivationFunction;
        };
    }
}
}

#include "../tcc/GRULayer.tcc"
