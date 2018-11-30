////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorNode.h (nodes)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ActivationLayerNode.h"
#include "BatchNormalizationLayerNode.h"
#include "BiasLayerNode.h"
#include "BinaryConvolutionalLayerNode.h"
#include "ConvolutionalLayerNode.h"
#include "FullyConnectedLayerNode.h"
#include "PoolingLayerNode.h"
#include "RegionDetectionLayerNode.h"
#include "ScalingLayerNode.h"
#include "SoftmaxLayerNode.h"

#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <predictors/include/NeuralNetworkPredictor.h>

// activation and pooling functions
#include <predictors/neural/include/LeakyReLUActivation.h>
#include <predictors/neural/include/MaxPoolingFunction.h>
#include <predictors/neural/include/MeanPoolingFunction.h>
#include <predictors/neural/include/ParametricReLUActivation.h>
#include <predictors/neural/include/ReLUActivation.h>
#include <predictors/neural/include/SigmoidActivation.h>

#include <utilities/include/TypeName.h>

#include <functional>
#include <string>
#include <type_traits>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that represents a neural network. </summary>
    template <typename ValueType>
    class NeuralNetworkPredictorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        using PredictorType = typename predictors::NeuralNetworkPredictor<ValueType>;
        using Layer = typename predictors::neural::Layer<ValueType>;

        /// <summary> Default Constructor </summary>
        NeuralNetworkPredictorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to predict from </param>
        /// <param name="predictor"> The predictor to use when making the prediction. </param>
        NeuralNetworkPredictorNode(const model::OutputPort<ValueType>& input, const PredictorType& predictor);

        /// <summary> Returns the underlying predictor </summary>
        ///
        /// <returns> The predictor wrapped by this node </returns>
        const PredictorType& GetPredictor() const { return _predictor; }

        /// <summary> Returns the underlying predictor </summary>
        ///
        /// <returns> The predictor wrapped by this node </returns>
        PredictorType& GetPredictor() { return _predictor; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("NeuralNetworkPredictorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Options to control how the network is compiled into nodes </summary>
        struct NetworkCompileOptions
        {
            /// <summary> Use diagonal convolution (vs. im2col-based convolution)
            bool useDiagonalConvolution;

            /// <summary> Ensure the output of the nodes implementing a layer is in the canonical row, column, channel order </summary>
            bool alwaysConvertToInterleaved;

            /// <summary> When using im2col-based convolution, construct the transpose of the receptive field matrix </summary>
            bool transposeReceptiveFieldMatrix;
        };

        struct NetworkCompileState
        {
            /// <summary> Indicates the current order of input data. If `true`, it's in the canonical row, column, channel order. </summary>
            bool isInterleavedOrder;
        };

        /// <summary> Reset the state of the node </summary>
        void Reset() override;

    protected:
        void Compute() const override;
        bool Refine(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        NeuralNetworkLayerNodeBase<ValueType>* AddLayerNode(model::ModelTransformer& transformer, Layer& layer, const model::OutputPort<ValueType>& layerInputs, const NetworkCompileOptions& options, NetworkCompileState& state) const;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Pointer to the predictor
        PredictorType _predictor;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    //
    // Helper functions
    //
    namespace
    {
        template <typename LayerType, typename LayerNodeType, typename ValueType>
        NeuralNetworkLayerNodeBase<ValueType>* TryAddLayerNode(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::OutputPort<ValueType>& layerInputs, const typename NeuralNetworkPredictorNode<ValueType>::NetworkCompileOptions& options, typename NeuralNetworkPredictorNode<ValueType>::NetworkCompileState& state)
        {
            auto typedLayer = dynamic_cast<LayerType*>(&layer);
            if (typedLayer != nullptr)
            {
                return transformer.AddNode<LayerNodeType>(layerInputs, *typedLayer);
            }
            return nullptr;
        }

        template <typename LayerType, typename LayerNodeType, typename SecondValueType, typename ValueType>
        NeuralNetworkLayerNodeBase<ValueType>* TryAddLayerNodeWithTwoInputs(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::OutputPort<ValueType>& layerInputs, const model::OutputPort<SecondValueType>& secondInput, const typename NeuralNetworkPredictorNode<ValueType>::NetworkCompileOptions& options, typename NeuralNetworkPredictorNode<ValueType>::NetworkCompileState& state)
        {
            auto typedLayer = dynamic_cast<LayerType*>(&layer);
            if (typedLayer != nullptr)
            {
                return transformer.AddNode<LayerNodeType>(layerInputs, secondInput, *typedLayer);
            }
            return nullptr;
        }
    } // namespace

    template <typename ValueType>
    NeuralNetworkLayerNodeBase<ValueType>* NeuralNetworkPredictorNode<ValueType>::AddLayerNode(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::OutputPort<ValueType>& layerInputs, const NetworkCompileOptions& options, NetworkCompileState& state) const
    {
        NeuralNetworkLayerNodeBase<ValueType>* node = nullptr;

        if (layer.template IsA<const predictors::neural::ActivationLayer<ValueType>>())
        {
            auto& activationLayer = layer.template As<predictors::neural::ActivationLayer<ValueType>>();
            auto paf = dynamic_cast<const predictors::neural::ParametricReLUActivation<ValueType>*>(activationLayer.GetActivationFunction().GetImpl());
            if (paf)
            {
                // Ah, then this one is special, we have to use ParametricReLUActivationLayerNode in this case.
                return TryAddLayerNode<predictors::neural::ActivationLayer<ValueType>, ParametricReLUActivationLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
            }
        }

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType>, ActivationLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::BatchNormalizationLayer<ValueType>, BatchNormalizationLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::BiasLayer<ValueType>, BiasLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::BinaryConvolutionalLayer<ValueType>, BinaryConvolutionalLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ConvolutionalLayer<ValueType>, ConvolutionalLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::FullyConnectedLayer<ValueType>, FullyConnectedLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        //
        // Pooling layer
        //

        node = TryAddLayerNode<predictors::neural::PoolingLayer<ValueType, predictors::neural::MaxPoolingFunction>, PoolingLayerNode<ValueType, predictors::neural::MaxPoolingFunction>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::PoolingLayer<ValueType, predictors::neural::MeanPoolingFunction>, PoolingLayerNode<ValueType, predictors::neural::MeanPoolingFunction>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::RegionDetectionLayer<ValueType>, RegionDetectionLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ScalingLayer<ValueType>, ScalingLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::SoftmaxLayer<ValueType>, SoftmaxLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        auto name = layer.GetRuntimeTypeName();
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unknown layer type in refine: " + name);
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
