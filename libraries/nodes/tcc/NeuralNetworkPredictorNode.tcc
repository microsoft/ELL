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
        model::Node* TryAddLayerNode(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::PortElements<ValueType>& layerInputs)
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
    model::Node* NeuralNetworkPredictorNode<ValueType>::AddLayerNode(model::ModelTransformer& transformer, predictors::neural::Layer<ValueType>& layer, const model::PortElements<ValueType>& layerInputs) const
    {
        model::Node* node = nullptr;

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType, predictors::neural::LeakyReLUActivation>, ActivationLayerNode<ValueType, predictors::neural::LeakyReLUActivation>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType, predictors::neural::ReLUActivation>, ActivationLayerNode<ValueType, predictors::neural::ReLUActivation>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ActivationLayer<ValueType, predictors::neural::SigmoidActivation>, ActivationLayerNode<ValueType, predictors::neural::SigmoidActivation>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::BatchNormalizationLayer<ValueType>, BatchNormalizationLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::BiasLayer<ValueType>, BiasLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::BinaryConvolutionalLayer<ValueType>, BinaryConvolutionalLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ConvolutionalLayer<ValueType>, ConvolutionalLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::FullyConnectedLayer<ValueType>, FullyConnectedLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::PoolingLayer<ValueType, predictors::neural::MaxPoolingFunction>, PoolingLayerNode<ValueType, predictors::neural::MaxPoolingFunction>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::PoolingLayer<ValueType, predictors::neural::MeanPoolingFunction>, PoolingLayerNode<ValueType, predictors::neural::MeanPoolingFunction>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::ScalingLayer<ValueType>, ScalingLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::SoftmaxLayer<ValueType>, SoftmaxLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        assert(false && "Unknown layer type in refine");
        return nullptr;
    }
}
}
