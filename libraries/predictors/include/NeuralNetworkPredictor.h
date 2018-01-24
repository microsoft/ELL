////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictor.h (predictors)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPredictor.h"

// math
#include "Vector.h"
#include "Matrix.h"

// datasets
#include "Dataset.h"

// neural network
#include "Layer.h"
#include "ActivationLayer.h"
#include "BatchNormalizationLayer.h"
#include "BiasLayer.h"
#include "BinaryConvolutionalLayer.h"
#include "ConvolutionalLayer.h"
#include "FullyConnectedLayer.h"
#include "GRULayer.h"
#include "HardSigmoidActivation.h"
#include "InputLayer.h"
#include "LeakyReLUActivation.h"
#include "LSTMLayer.h"
#include "MaxPoolingFunction.h"
#include "MeanPoolingFunction.h"
#include "ParametricReLUActivation.h"
#include "PoolingLayer.h"
#include "RecurrentLayer.h"
#include "RegionDetectionLayer.h"
#include "ReLUActivation.h"
#include "ScalingLayer.h"
#include "SigmoidActivation.h"
#include "SoftmaxLayer.h"
#include "TanhActivation.h"

// utilities
#include "IArchivable.h"

// stl
#include <cstddef>
#include <memory>


namespace ell
{
namespace predictors
{
    /// <summary> A neural network predictor. </summary>
    template <typename ElementType>
    class NeuralNetworkPredictor : public IPredictor<std::vector<ElementType>>, public utilities::IArchivable
    {
    public:
        /// <summary> Type of the input vector expected by this predictor type. </summary>
        using DataVectorType = typename neural::Layer<ElementType>::DataVectorType;

        /// <summary> Dimensions of the input and output tensors. </summary>
        using Shape = typename neural::Layer<ElementType>::Shape;

        /// <summary> A unique_ptr to the input layer for this predictor. </summary>
        using InputLayerReference = std::shared_ptr<neural::InputLayer<ElementType>>;

        /// <summary> A vector of layers. </summary>
        using Layers = std::vector<std::shared_ptr<neural::Layer<ElementType>>>;

        NeuralNetworkPredictor() = default;
        NeuralNetworkPredictor(const NeuralNetworkPredictor&) = default;

        /// <summary> Constructs an instance of NerualNetworkPredictor. </summary>
        ///
        /// <param name="inputLayer"> The input layer for this network. </param>
        /// <param name="layers"> The substantive layers comprising this network. The first layer
        /// in this vector receives its input from the input layer. </param>
        NeuralNetworkPredictor(InputLayerReference&& inputLayer, Layers&& layers);

        /// <summary> Returns the input layer. </summary>
        ///
        /// <returns> The underlying input layer. </returns>
        const neural::InputLayer<ElementType>& GetInputLayer() const { return *_inputLayer; }

        /// <summary> Returns the number of underlying layers. </summary>
        ///
        /// <returns> The underlying vector of layers. </returns>
        size_t NumLayers() const { return _layers.size(); }

        /// <summary> Returns the underlying layers. </summary>
        ///
        /// <returns> The underlying vector of layers. </returns>
        const Layers& GetLayers() const { return _layers; }

        /// <summary> Sets the underlying layers. </summary>
        ///
        /// <returns> The underlying vector of layers. </returns>
        void SetLayers(Layers&& layers) { _layers = std::move(layers); } // STYLE discrepancy

        /// <summary> Removes layers from the end of the neural network.  </summary>
        ///
        /// <param name="numberToRemove"> The number of layers to remove from the end of the neural network. </param>
        void RemoveLastLayers(size_t numberToRemove = 1);

        /// <summary> Gets the dimension of the input layer. </summary>
        ///
        /// <returns> The dimension. </returns>
        Shape GetInputShape() const;

        /// <summary> Gets the dimension of the output layer. </summary>
        ///
        /// <returns> The dimension. </returns>
        Shape GetOutputShape() const;

        /// <summary> Returns the output of the network for a given input. </summary>
        ///
        /// <param name="input"> The data vector. </param>
        ///
        /// <returns> The prediction. </returns>
        const std::vector<ElementType>& Predict(const DataVectorType& dataVector) const;

        /// <summary> Returns the output of the network for a given input. </summary>
        ///
        /// <param name="input"> The input data. </param>
        ///
        /// <returns> The prediction. </returns>
        const std::vector<ElementType>& Predict(const std::vector<ElementType>& input) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("NeuralNetworkPredictor"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Register known types for neural network predictors to a serialization context </summary>
        ///
        /// <param name="context"> The `SerializationContext` </param>
        static void RegisterNeuralNetworkPredictorTypes(utilities::SerializationContext& context);

        /// <summary> Gets the current archive format version. </summary>
        ///
        /// <returns> The current archive format version. </summary>
        static utilities::ArchiveVersion GetCurrentArchiveVersion();

    protected:
        utilities::ArchiveVersion GetArchiveVersion() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Compute() const;
        InputLayerReference _inputLayer;
        Layers _layers;
        mutable std::vector<ElementType> _output;
    };
}
}

#include "../tcc/NeuralNetworkPredictor.tcc"
