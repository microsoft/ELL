////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LSTMLayer.h (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "HardSigmoidActivation.h"
#include "Layer.h"
#include "TanhActivation.h"
#include "SigmoidActivation.h"

// math
#include "Matrix.h"

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Specifies the parameters of the LSTM layer. </summary>
        template <typename ElementType>
        struct LSTMParameters
        {
            typename Layer<ElementType>::MatrixType& inputWeights;
            typename Layer<ElementType>::MatrixType& forgetMeWeights;
            typename Layer<ElementType>::MatrixType& candidateWeights;
            typename Layer<ElementType>::MatrixType& outputWeights;

            typename Layer<ElementType>::VectorType& inputBias;
            typename Layer<ElementType>::VectorType& forgetMeBias;
            typename Layer<ElementType>::VectorType& candidateBias;
            typename Layer<ElementType>::VectorType& outputBias;
        };

        /// <summary>
        /// A layer in a recurrent network that implements an LSTM layer. This layer retains "memory"
        /// over time and uses this information to inform predictions.
        /// </summary>
        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        class LSTMLayer : public Layer<ElementType>
        {
        public:
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using VectorType = typename Layer<ElementType>::VectorType;
            using MatrixType = typename Layer<ElementType>::MatrixType;
            using ConstMatrixReferenceType = typename Layer<ElementType>::ConstMatrixReferenceType;
            using ConstTensorReferenceType = typename Layer<ElementType>::ConstTensorReferenceType;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::NumOutputRowsMinusPadding;
            using Layer<ElementType>::NumOutputColumnsMinusPadding;
            using Layer<ElementType>::NumOutputChannels;

            /// <summary> Default constructor. </summary>
            LSTMLayer();

            /// <summary> Instantiates an instance of a LSTM layer. </summary>
            ///
            /// <param name="layerParameters"> The parameters common to every layer. </param>
            /// <param name="parameters"> The weights and biases applicable to an LSTM. Weights should be organised as: [weights, recurrent layer weights] or [W, U].
            /// Biases should be compatible in dimensionality with the output of the network.</param>
            LSTMLayer(const LayerParameters& layerParameters, LSTMParameters<ElementType>& parameters);

            /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::lstm; }

            /// <summary> Retrieves the weights applied to the input layer </summary>
            ///
            /// <returns> A matrix of weights. </returns>
            const MatrixType& GetInputWeights() const { return _inputWeights; }

            /// <summary> Retrieves the weights applied to the forgotten layer </summary>
            ///
            /// <returns> A matrix of weights. </returns>
            const MatrixType& GetForgetMeWeights() const { return _forgetMeWeights; }

            /// <summary> Retrieves the weights applied to the candidate layer </summary>
            ///
            /// <returns> A matrix of weights. </returns>
            const MatrixType& GetCandidateWeights() const { return _candidateWeights; }

            /// <summary> Retrieves the weights applied to the output layer </summary>
            ///
            /// <returns> A matrix of weights. </returns>
            const MatrixType& GetOutputWeights() const { return _outputWeights; }

            /// <summary> Retrieves the biases applied to the input nodes </summary>
            ///
            /// <returns> A vector of biases. </returns>
            const VectorType& GetInputBias() const { return _inputBias; }

            /// <summary> Retrieves the biases applied to forgotten layer nodes </summary>
            ///
            /// <returns> A vector of biases. </returns>
            const VectorType& GetForgetMeBias() const { return _forgetMeBias; }

            /// <summary> Retrieves the biases applied to candidate layer nodes </summary>
            ///
            /// <returns> A vector of biases. </returns>
            const VectorType& GetCandidateBias() const { return _candidateBias; }

            /// <summary> Retrieves the biases applied to output layer nodes </summary>
            ///
            /// <returns> A vector of biases. </returns>
            const VectorType& GetOutputBias() const { return _outputBias; }

            /// <summary> Resets the layer's hidden values </summary>
            void Reset();

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType, ActivationFunctionType<ElementType>, RecurrentActivationFunctionType<ElementType>>("LSTMLayer"); }

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

            MatrixType _inputWeights;
            MatrixType _forgetMeWeights;
            MatrixType _candidateWeights;
            MatrixType _outputWeights;

            VectorType _inputBias;
            VectorType _forgetMeBias;
            VectorType _candidateBias;
            VectorType _outputBias;

            // Stored state
            VectorType _inputPlusHiddenVector;
            VectorType _ctActual;

            ActivationFunctionType<ElementType> _activationFunction;
            RecurrentActivationFunctionType<ElementType> _recurrentActivationFunction;
        };
    }
}
}

#include "../tcc/LSTMLayer.tcc"
