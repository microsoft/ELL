////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorNode.tcc (nodes)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
        NeuralNetworkLayerNodeBase<ValueType>* TryAddLayerNode(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::PortElements<ValueType>& layerInputs, const typename NeuralNetworkPredictorNode<ValueType>::NetworkCompileOptions& options, typename NeuralNetworkPredictorNode<ValueType>::NetworkCompileState& state)
        {
            auto typedLayer = dynamic_cast<LayerType*>(&layer);
            if (typedLayer != nullptr)
            {
                return transformer.AddNode<LayerNodeType>(layerInputs, *typedLayer);
            }
            return nullptr;
        }
    }

    template <typename ValueType>
    NeuralNetworkLayerNodeBase<ValueType>* NeuralNetworkPredictorNode<ValueType>::AddLayerNode(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::PortElements<ValueType>& layerInputs, const NetworkCompileOptions& options, NetworkCompileState& state) const
    {
        NeuralNetworkLayerNodeBase<ValueType>* node = nullptr;

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType, predictors::neural::LeakyReLUActivation>, ActivationLayerNode<ValueType, predictors::neural::LeakyReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType, predictors::neural::ParametricReLUActivation>, ParametricReLUActivationLayerNode<ValueType>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType, predictors::neural::ReLUActivation>, ActivationLayerNode<ValueType, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType, predictors::neural::SigmoidActivation>, ActivationLayerNode<ValueType, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType, predictors::neural::HardSigmoidActivation>, ActivationLayerNode<ValueType, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
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
        // GRULayer's with an inner product of ['TanhActivation', 'SigmoidActivation', 'HardSigmoidActivation', 'ReLUActivation']
        //

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::TanhActivation, predictors::neural::TanhActivation>, GRULayerNode<ValueType, predictors::neural::TanhActivation, predictors::neural::TanhActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::TanhActivation, predictors::neural::SigmoidActivation>, GRULayerNode<ValueType, predictors::neural::TanhActivation, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::TanhActivation, predictors::neural::HardSigmoidActivation>, GRULayerNode<ValueType, predictors::neural::TanhActivation, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::TanhActivation, predictors::neural::ReLUActivation>, GRULayerNode<ValueType, predictors::neural::TanhActivation, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::SigmoidActivation, predictors::neural::TanhActivation>, GRULayerNode<ValueType, predictors::neural::SigmoidActivation, predictors::neural::TanhActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::SigmoidActivation, predictors::neural::SigmoidActivation>, GRULayerNode<ValueType, predictors::neural::SigmoidActivation, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::SigmoidActivation, predictors::neural::HardSigmoidActivation>, GRULayerNode<ValueType, predictors::neural::SigmoidActivation, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::SigmoidActivation, predictors::neural::ReLUActivation>, GRULayerNode<ValueType, predictors::neural::SigmoidActivation, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::TanhActivation>, GRULayerNode<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::TanhActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::SigmoidActivation>, GRULayerNode<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::HardSigmoidActivation>, GRULayerNode<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::ReLUActivation>, GRULayerNode<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::ReLUActivation, predictors::neural::TanhActivation>, GRULayerNode<ValueType, predictors::neural::ReLUActivation, predictors::neural::TanhActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::ReLUActivation, predictors::neural::SigmoidActivation>, GRULayerNode<ValueType, predictors::neural::ReLUActivation, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::ReLUActivation, predictors::neural::HardSigmoidActivation>, GRULayerNode<ValueType, predictors::neural::ReLUActivation, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::GRULayer<ValueType, predictors::neural::ReLUActivation, predictors::neural::ReLUActivation>, GRULayerNode<ValueType, predictors::neural::ReLUActivation, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        //
        // LSTMLayer's with an inner product of ['TanhActivation', 'SigmoidActivation', 'HardSigmoidActivation', 'ReLUActivation']
        //

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::TanhActivation, predictors::neural::TanhActivation>, LSTMLayerNode<ValueType, predictors::neural::TanhActivation, predictors::neural::TanhActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::TanhActivation, predictors::neural::SigmoidActivation>, LSTMLayerNode<ValueType, predictors::neural::TanhActivation, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::TanhActivation, predictors::neural::HardSigmoidActivation>, LSTMLayerNode<ValueType, predictors::neural::TanhActivation, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::TanhActivation, predictors::neural::ReLUActivation>, LSTMLayerNode<ValueType, predictors::neural::TanhActivation, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::SigmoidActivation, predictors::neural::TanhActivation>, LSTMLayerNode<ValueType, predictors::neural::SigmoidActivation, predictors::neural::TanhActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::SigmoidActivation, predictors::neural::SigmoidActivation>, LSTMLayerNode<ValueType, predictors::neural::SigmoidActivation, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::SigmoidActivation, predictors::neural::HardSigmoidActivation>, LSTMLayerNode<ValueType, predictors::neural::SigmoidActivation, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::SigmoidActivation, predictors::neural::ReLUActivation>, LSTMLayerNode<ValueType, predictors::neural::SigmoidActivation, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::TanhActivation>, LSTMLayerNode<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::TanhActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::SigmoidActivation>, LSTMLayerNode<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::HardSigmoidActivation>, LSTMLayerNode<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::ReLUActivation>, LSTMLayerNode<ValueType, predictors::neural::HardSigmoidActivation, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::ReLUActivation, predictors::neural::TanhActivation>, LSTMLayerNode<ValueType, predictors::neural::ReLUActivation, predictors::neural::TanhActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::ReLUActivation, predictors::neural::SigmoidActivation>, LSTMLayerNode<ValueType, predictors::neural::ReLUActivation, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::ReLUActivation, predictors::neural::HardSigmoidActivation>, LSTMLayerNode<ValueType, predictors::neural::ReLUActivation, predictors::neural::HardSigmoidActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::LSTMLayer<ValueType, predictors::neural::ReLUActivation, predictors::neural::ReLUActivation>, LSTMLayerNode<ValueType, predictors::neural::ReLUActivation, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs, options, state);
        if (node != nullptr) return node;

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
}
}
