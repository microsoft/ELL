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

        template <typename LayerType, typename LayerNodeType, typename SecondValueType, typename ValueType>
        NeuralNetworkLayerNodeBase<ValueType>* TryAddLayerNodeWithTwoInputs(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::PortElements<ValueType>& layerInputs, const model::PortElements<SecondValueType>& secondInput, const typename NeuralNetworkPredictorNode<ValueType>::NetworkCompileOptions& options, typename NeuralNetworkPredictorNode<ValueType>::NetworkCompileState& state)
        {
            auto typedLayer = dynamic_cast<LayerType*>(&layer);
            if (typedLayer != nullptr)
            {
                return transformer.AddNode<LayerNodeType>(layerInputs, secondInput, *typedLayer);
            }
            return nullptr;
        }
    }

    template <typename ValueType>
    NeuralNetworkLayerNodeBase<ValueType>* NeuralNetworkPredictorNode<ValueType>::AddLayerNode(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::PortElements<ValueType>& layerInputs, const NetworkCompileOptions& options, NetworkCompileState& state) const
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

        // todo: figure out how to pass a resetTrigger as second input to the GRULayer (e.g. could be output of VoiceActivityDetectorNode).
        // for now the user can skip Layers and use GRULayerNode directly and that way they can provide the reset input.
        auto resetTriggerNode = transformer.AddNode<ConstantNode<int>>(0);

        node = TryAddLayerNodeWithTwoInputs<predictors::neural::GRULayer<ValueType>, GRULayerNode<ValueType>, int>(transformer, layer, layerInputs, resetTriggerNode->output, options, state);
        if (node != nullptr) return node;

        node = TryAddLayerNodeWithTwoInputs<predictors::neural::LSTMLayer<ValueType>, LSTMLayerNode<ValueType>, int>(transformer, layer, layerInputs, resetTriggerNode->output, options, state);
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
}
}
