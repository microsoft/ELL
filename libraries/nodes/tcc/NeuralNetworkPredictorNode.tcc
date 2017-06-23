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

        // node = TryAddLayerNode<predictors::neural::ActivationLayer<predictors::neural::LeakyReLUActivation>, ActivationLayerNode<predictors::neural::LeakyReLUActivation, ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::ActivationLayer<predictors::neural::ReLUActivation>, ActivationLayerNode<predictors::neural::ReLUActivation, ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::ActivationLayer<predictors::neural::SigmoidActivation>, ActivationLayerNode<predictors::neural::SigmoidActivation, ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::ActivationLayer<predictors::neural::SoftMaxActivation>, ActivationLayerNode<predictors::neural::SoftMaxActivation, ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::BatchNormalizationLayer<ValueType>, BatchNormalizationLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        node = TryAddLayerNode<predictors::neural::BiasLayer<ValueType>, BiasLayerNode<ValueType>>(transformer, layer, layerInputs);
        if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::BinaryConvolutionalLayer, BinaryConvolutionalLayerNode<ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::ConvolutionalLayer, ConvolutionalLayerNode<ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::FullyConnectedLayer, FullyConnectedLayerNode<ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::InputLayer, InputLayerNode<ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::PoolingLayer<predictors::neural::MaxPoolingFunction>, PoolingLayerNode<predictors::neural::MaxPoolingFunction, ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::PoolingLayer<predictors::neural::MeanPoolingFunction>, PoolingLayerNode<predictors::neural::MeanPoolingFunction, ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        // node = TryAddLayerNode<predictors::neural::ScalingLayer, ScalingLayerNode<ValueType>>(transformer, layer, layerInputs);
        // if (node != nullptr) return node;

        //assert(false && "Unknown layer type in refine");
        return nullptr;
    }
}
}
