////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelUtils.cpp (finetune)
//  Authors:  Byron Changuion, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelUtils.h"

#include <common/include/LoadModel.h>

#include <model/include/InputNode.h>
#include <model/include/Map.h>
#include <model/include/OutputNode.h>

#include <nodes/include/ClockNode.h>
#include <nodes/include/ActivationFunctions.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/NeuralNetworkPredictorNode.h>
#include <nodes/include/SinkNode.h>
#include <nodes/include/SourceNode.h>

using namespace ell;
using namespace ell::model;

namespace
{
math::TensorShape ToTensorShape(utilities::MemoryShape shape)
{
    shape.Resize(3);
    return math::TensorShape(shape[0], shape[1], shape[2]);
}
} // namespace

void SaveModel(const Model& model, const OutputPortBase& output, std::string filename)
{
    ModelTransformer transformer;
    TransformContext context;
    Submodel submodel(model, {}, { &output });
    auto prunedSubmodel = transformer.CopySubmodel(submodel, context);
    common::SaveModel(prunedSubmodel.GetModel(), filename);
}

// Querying nodes
bool IsSourceNode(const model::Node* node)
{
    return typeid(*node) == typeid(model::SourceNodeBase);
}

bool IsSinkNode(const model::Node* node)
{
    return typeid(*node) == typeid(model::SinkNodeBase);
}

bool IsClockNode(const model::Node* node)
{
    return typeid(*node) == typeid(nodes::ClockNode);
}

bool IsNeuralNetworkPredictorNode(const Node* node)
{
    return IsNeuralNetworkPredictorNode<float>(node) || IsNeuralNetworkPredictorNode<double>(node);
}

bool IsFullyConnectedLayerNode(const Node* node)
{
    return IsFullyConnectedLayerNode<float>(node) || IsFullyConnectedLayerNode<double>(node);
}

bool IsBiasLayerNode(const Node* node)
{
    return IsBiasLayerNode<float>(node) || IsBiasLayerNode<double>(node);
}

bool IsConvolutionalLayerNode(const Node* node)
{
    return IsConvolutionalLayerNode<float>(node) || IsConvolutionalLayerNode<double>(node);
}

template <typename T>
bool IsSourceNode(const model::Node* node)
{
    return typeid(*node) == typeid(nodes::SourceNode<T>);
}

template <typename T>
bool IsSinkNode(const model::Node* node)
{
    return typeid(*node) == typeid(nodes::SinkNode<T>);
}

template <typename T>
bool IsNeuralNetworkPredictorNode(const Node* node)
{
    return typeid(*node) == typeid(nodes::NeuralNetworkPredictorNode<T>);
}

template <typename T>
bool IsFullyConnectedLayerNode(const Node* node)
{
    return typeid(*node) == typeid(nodes::FullyConnectedLayerNode<T>);
}

template <typename T>
bool IsBiasLayerNode(const Node* node)
{
    return typeid(*node) == typeid(nodes::BiasLayerNode<T>);
}

template <typename T>
bool IsConvolutionalLayerNode(const Node* node)
{
    return typeid(*node) == typeid(nodes::ConvolutionalLayerNode<T>);
}

// Finding things
InputNodeBase* GetInputNode(Model& model, const model::OutputPortBase& output)
{
    auto iter = model.GetNodeIterator(&output);
    while (iter.IsValid())
    {
        auto node = iter.Get();
        auto inputNode = dynamic_cast<const InputNodeBase*>(node);
        if (inputNode)
        {
            return const_cast<InputNodeBase*>(inputNode);
        }
        iter.Next();
    }

    throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find input node in model");
}

template <typename ElementType>
InputNode<ElementType>* GetInputNode(Model& model, const model::OutputPortBase& output)
{
    auto iter = model.GetNodeIterator(&output);
    while (iter.IsValid())
    {
        auto node = iter.Get();
        auto inputNode = dynamic_cast<const InputNode<ElementType>*>(node);
        if (inputNode)
        {
            return const_cast<InputNode<ElementType>*>(inputNode);
        }
        iter.Next();
    }

    throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find input node in model");
}

const OutputNodeBase* GetOutputNode(const Model& model)
{
    auto nodes = model.GetNodesByType<OutputNodeBase>();
    if (nodes.size() < 1)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find output node in model");
    }
    return nodes[0];
}

template <typename ElementType>
const OutputNode<ElementType>* GetOutputNode(const Model& model)
{
    auto nodes = model.GetNodesByType<OutputNode<ElementType>>();
    if (nodes.size() < 1)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find output node in model");
    }
    return nodes[0];
}

template <typename ElementType>
std::vector<const nodes::FullyConnectedLayerNode<ElementType>*> GetFullyConnectedLayerNodes(const Model& model, const OutputPortBase& output)
{
    std::vector<const nodes::FullyConnectedLayerNode<ElementType>*> result;
    model.VisitSubmodel(&output, [&result](const Node& node) {
        if (IsFullyConnectedLayerNode<ElementType>(&node))
        {
            result.push_back(static_cast<const nodes::FullyConnectedLayerNode<ElementType>*>(&node));
        }
    });
    return result;
}

template <typename ElementType>
std::vector<const nodes::BiasLayerNode<ElementType>*> GetBiasLayerNodes(const Model& model, const OutputPortBase& output)
{
    std::vector<const nodes::BiasLayerNode<ElementType>*> result;
    model.VisitSubmodel(&output, [&result](const Node& node) {
        if (IsBiasLayerNode<ElementType>(&node))
        {
            result.push_back(static_cast<const nodes::BiasLayerNode<ElementType>*>(&node));
        }
    });
    return result;
}

template <typename ElementType>
std::vector<const nodes::ConvolutionalLayerNode<ElementType>*> GetConvolutionalLayerNodes(const Model& model, const OutputPortBase& output)
{
    std::vector<const nodes::ConvolutionalLayerNode<ElementType>*> result;
    model.VisitSubmodel(&output, [&result](const Node& node) {
        if (IsConvolutionalLayerNode<ElementType>(&node))
        {
            result.push_back(static_cast<const nodes::ConvolutionalLayerNode<ElementType>*>(&node));
        }
    });
    return result;
}

const std::vector<const OutputPortBase*> GetParents(const OutputPortBase* port)
{
    std::vector<const OutputPortBase*> result;
    auto node = port->GetNode();
    for (auto input : node->GetInputPorts())
    {
        result.push_back(&input->GetReferencedPort());
    }
    return result;
}

// Map-related
template <typename ElementType>
Map MakeNeuralNetworkPredictorMap(const typename nodes::NeuralNetworkPredictorNode<ElementType>::PredictorType& predictor)
{
    Model model;
    MemoryShape inputShape(predictor.GetInputShape());
    auto inputNode = model.AddNode<InputNode<ElementType>>(inputShape);
    auto newPredictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, predictor);

    return Map(model, { { "input", inputNode } }, { { "output", newPredictorNode->output } });
}

Map GetMapPrefix(const Map& map, const std::string& outputElementsString)
{
    auto& model = map.GetModel();
    auto parsedOutputElements = ParsePortElementsProxy(outputElementsString);
    auto outputElements = ProxyToPortElements(model, parsedOutputElements);
    const auto& input = map.GetInput();
    return Map(model, { { "input", input } }, { { "output", outputElements } });
}

//
// Model-related
//
template <typename ElementType>
const OutputPort<ElementType>& AppendOutputWithSink(Model& model, const OutputPort<ElementType>& output)
{
    const auto& sinkOutput = AppendSink<ElementType>(model, output);
    auto outputNode = model.AddNode<OutputNode<ElementType>>(sinkOutput);
    return outputNode->output;
}

template <typename ElementType>
const OutputPort<ElementType>& AppendSink(Model& model, const OutputPort<ElementType>& output)
{
    auto condition = model.AddNode<nodes::ConstantNode<bool>>(true);
    auto sinkNode = model.AddNode<nodes::SinkNode<ElementType>>(output, condition->output, "OutputCallback");
    return sinkNode->output;
}

template <typename ElementType>
const OutputPort<ElementType>& AppendBiasLayer(Model& model, const OutputPort<ElementType>& output, math::ConstColumnVectorReference<ElementType> bias)
{
    return AppendBiasLayer(model, output, bias.ToArray());
}

template <typename ElementType>
const OutputPort<ElementType>& AppendBiasLayer(Model& model, const OutputPort<ElementType>& output, const std::vector<ElementType>& bias)
{
    auto shape = ToTensorShape(output.GetMemoryLayout().GetActiveSize());
    typename predictors::neural::Layer<ElementType>::TensorType inputTensorPlaceholder(shape);
    typename predictors::neural::Layer<ElementType>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<ElementType>::LayerParameters biasParams{ inputTensorPlaceholderRef, predictors::neural::NoPadding(), shape, predictors::neural::NoPadding() };
    auto biasVector = math::ColumnVector<ElementType>(bias);
    auto biasRef = biasVector.GetConstReference();
    predictors::neural::BiasLayer<ElementType> biasLayer(biasParams, biasRef);
    auto biasNode = model.AddNode<nodes::BiasLayerNode<ElementType>>(output, biasLayer);
    return biasNode->output;
}

template <typename ElementType>
const OutputPort<ElementType>& AppendScalingLayer(Model& model, const OutputPort<ElementType>& output, math::ConstColumnVectorReference<ElementType> scale)
{
    return AppendScalingLayer(model, output, scale.ToArray());
}

template <typename ElementType>
const OutputPort<ElementType>& AppendScalingLayer(Model& model, const OutputPort<ElementType>& output, const std::vector<ElementType>& scale)
{
    auto inputShape = output.GetMemoryLayout().GetActiveSize();
    typename predictors::neural::Layer<ElementType>::TensorType inputTensorPlaceholder(ToTensorShape(inputShape));
    typename predictors::neural::Layer<ElementType>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<ElementType>::LayerParameters scaleParams{ inputTensorPlaceholderRef, predictors::neural::NoPadding(), ToTensorShape(inputShape), predictors::neural::NoPadding() };
    auto scaleVector = math::ColumnVector<ElementType>(scale);
    auto scaleRef = scaleVector.GetConstReference();
    predictors::neural::ScalingLayer<ElementType> scaleLayer(scaleParams, scaleRef);
    auto scaleNode = model.AddNode<nodes::ScalingLayerNode<ElementType>>(output, scaleLayer);
    return scaleNode->output;
}

template <typename ElementType>
const OutputPort<ElementType>& AppendFullyConnectedLayer(Model& model, const OutputPort<ElementType>& output, math::ConstRowMatrixReference<ElementType> weights)
{
    typename predictors::neural::Layer<ElementType>::TensorType inputTensorPlaceholder(1, 1, output.Size());
    typename predictors::neural::Layer<ElementType>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<ElementType>::LayerParameters fullyConnectedParams{ inputTensorPlaceholderRef, predictors::neural::NoPadding(), { 1, 1, weights.NumRows() }, predictors::neural::NoPadding() };
    predictors::neural::FullyConnectedLayer<ElementType> fullyConnectedLayer(fullyConnectedParams, weights);
    auto fullyConnectedNode = model.AddNode<nodes::FullyConnectedLayerNode<ElementType>>(output, fullyConnectedLayer);
    return fullyConnectedNode->output;
}

// TODO: rename 'output' param here --- it's the input to the convolutional layer
// TODO: add implicit padding
template <typename ElementType>
const OutputPort<ElementType>& AppendConvolutionalLayer(Model& model, const OutputPort<ElementType>& output, int filterSize, int stride, int inputPadding, int outputPadding, math::ConstRowMatrixReference<ElementType> weights)
{
    //    const auto inputShape = output.GetMemoryLayout().GetActiveSize();
    const auto inputShape = output.GetMemoryLayout().GetExtent();
    const auto numRows = inputShape[0];
    const auto numColumns = inputShape[1];
    const auto numChannels = inputShape[2];

    const auto numOutputRows = (numRows - filterSize + 1) / stride;
    const auto numOutputColumns = (numColumns - filterSize + 1) / stride;
    const auto numOutputChannels = weights.NumRows();

    // TODO: pass in original layer's output shape, for verification? (or, compare output of this function with original layer output shape)

    typename predictors::neural::Layer<ElementType>::TensorType inputTensorPlaceholder(ToTensorShape(inputShape));
    typename predictors::neural::Layer<ElementType>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<ElementType>::LayerParameters layerParams{ inputTensorPlaceholderRef, predictors::neural::ZeroPadding(inputPadding), { static_cast<size_t>(numOutputRows), static_cast<size_t>(numOutputColumns), static_cast<size_t>(numOutputChannels) }, predictors::neural::ZeroPadding(outputPadding) };
    predictors::neural::ConvolutionalParameters convParams{ static_cast<size_t>(filterSize), static_cast<size_t>(stride), predictors::neural::ConvolutionMethod::automatic, 1 };
    math::ChannelColumnRowTensor<ElementType> weightsTensor(static_cast<size_t>(numOutputChannels * filterSize), static_cast<size_t>(filterSize), numChannels, weights.ToArray());
    predictors::neural::ConvolutionalLayer<ElementType> convLayer(layerParams, convParams, weightsTensor);
    auto convNode = model.AddNode<nodes::ConvolutionalLayerNode<ElementType>>(output, convLayer);
    return convNode->output;
}

// TODO: rename 'output' parameter -- it's where we graft the new node
template <typename ElementType>
const OutputPort<ElementType>& AppendFineTunedFullyConnectedNodes(Model& model, const OutputPort<ElementType>& output, const WeightsAndBias<ElementType>& weightsAndBias)
{
    auto weightsRef = weightsAndBias.weights.GetConstReference();
    auto biasRef = weightsAndBias.bias.GetConstReference();
    const auto& fc = AppendFullyConnectedLayer(model, output, weightsRef);
    const auto& bias = AppendBiasLayer(model, fc, biasRef);
    return bias;
}

// TODO: rename 'output' parameter -- it's where we graft the new node
template <typename ElementType>
const OutputPort<ElementType>& AppendFineTunedConvolutionalNodes(Model& model, const OutputPort<ElementType>& output, int filterSize, int stride, int inputPadding, int outputPadding, const WeightsAndBias<ElementType>& weightsAndBias)
{
    auto weightsRef = weightsAndBias.weights.GetConstReference();
    auto biasRef = weightsAndBias.bias.GetConstReference();
    const auto& conv = AppendConvolutionalLayer(model, output, filterSize, stride, inputPadding, outputPadding, weightsRef);
    const auto& bias = AppendBiasLayer(model, conv, biasRef);
    return bias;
}

void ReplaceSourceNode(ModelTransformer& transformer, const model::SourceNodeBase& n);
void ReplaceSinkNode(ModelTransformer& transformer, const model::SinkNodeBase& n);
const model::OutputPortBase& RemoveSourceAndSinkNodes(model::Model& model, const model::OutputPortBase& output);

template <typename ElementType>
const model::OutputPort<ElementType>& RemoveSourceAndSinkNodes(model::Model& model, const model::OutputPort<ElementType>& output)
{
    const auto& result = RemoveSourceAndSinkNodes(model, static_cast<const model::OutputPortBase&>(output));
    return static_cast<const model::OutputPort<ElementType>&>(result);
}

const model::OutputPortBase& RemoveSourceAndSinkNodes(model::Model& model, const model::OutputPortBase& output)
{
    TransformContext context;
    ModelTransformer transformer;
    Submodel submodel(model, {}, { &output });
    auto newSubmodel = transformer.TransformSubmodelOnto(submodel, {}, context, [](const Node& n, ModelTransformer& transformer) {
        if (IsSourceNode(&n))
        {
            ReplaceSourceNode(transformer, static_cast<const model::SourceNodeBase&>(n));
        }
        else if (IsSinkNode(&n))
        {
            ReplaceSinkNode(transformer, static_cast<const model::SinkNodeBase&>(n));
        }
        else
        {
            transformer.CopyNode(n);
        }
    });

    return *submodel.GetOutputs()[0];
}

template <typename ElementType>
void ReplaceSourceNode(ModelTransformer& transformer, const nodes::SourceNode<ElementType>& n);
void ReplaceSourceNode(ModelTransformer& transformer, const model::SourceNodeBase& n)
{
    switch (n.GetOutputType())
    {
    case Port::PortType::smallReal:
        ReplaceSourceNode(transformer, static_cast<const nodes::SourceNode<float>&>(n));
        break;
    case Port::PortType::real:
        ReplaceSourceNode(transformer, static_cast<const nodes::SourceNode<double>&>(n));
        break;
    case Port::PortType::integer:
        ReplaceSourceNode(transformer, static_cast<const nodes::SourceNode<int>&>(n));
        break;
    case Port::PortType::bigInt:
        ReplaceSourceNode(transformer, static_cast<const nodes::SourceNode<int64_t>&>(n));
        break;
    case Port::PortType::boolean:
        ReplaceSourceNode(transformer, static_cast<const nodes::SourceNode<bool>&>(n));
        break;
    default:
        throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
    }
}

template <typename ElementType>
void ReplaceSourceNode(ModelTransformer& transformer, const nodes::SourceNode<ElementType>& n)
{
    auto inputNode = transformer.AddNode<model::InputNode<ElementType>>(n.GetMemoryLayout());
    transformer.MapNodeOutput(n.output, inputNode->output);
}

template <typename ElementType>
void ReplaceSinkNode(ModelTransformer& transformer, const nodes::SinkNode<ElementType>& n);
void ReplaceSinkNode(ModelTransformer& transformer, const model::SinkNodeBase& n)
{
    switch (n.GetOutputPort().GetType())
    {
    case Port::PortType::smallReal:
        ReplaceSinkNode(transformer, static_cast<const nodes::SinkNode<float>&>(n));
        break;
    case Port::PortType::real:
        ReplaceSinkNode(transformer, static_cast<const nodes::SinkNode<double>&>(n));
        break;
    case Port::PortType::integer:
        ReplaceSinkNode(transformer, static_cast<const nodes::SinkNode<int>&>(n));
        break;
    case Port::PortType::bigInt:
        ReplaceSinkNode(transformer, static_cast<const nodes::SinkNode<int64_t>&>(n));
        break;
    case Port::PortType::boolean:
        ReplaceSinkNode(transformer, static_cast<const nodes::SinkNode<bool>&>(n));
        break;
    default:
        throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
    }
}

template <typename ElementType>
void ReplaceSinkNode(ModelTransformer& transformer, const nodes::SinkNode<ElementType>& n)
{
    const auto& input = transformer.GetCorrespondingInputs(n.input);
    auto outputNode = transformer.AddNode<model::OutputNode<ElementType>>(input);
    transformer.MapNodeOutput(n.output, outputNode->output);
}

#define INSTANTIATE_FUNCTIONS(TYPE)                                                                                                                                                                                                \
    template InputNode<TYPE>* GetInputNode(Model& model, const OutputPortBase& output);                                                                                                                                            \
    template const OutputNode<TYPE>* GetOutputNode(const Model& model);                                                                                                                                                            \
    template std::vector<const nodes::FullyConnectedLayerNode<TYPE>*> GetFullyConnectedLayerNodes(const Model& model, const OutputPortBase& output);                                                                               \
    template std::vector<const nodes::BiasLayerNode<TYPE>*> GetBiasLayerNodes(const Model& model, const OutputPortBase& output);                                                                                                   \
    template std::vector<const nodes::ConvolutionalLayerNode<TYPE>*> GetConvolutionalLayerNodes(const Model& model, const OutputPortBase& output);                                                                                 \
    template const OutputPort<TYPE>& AppendOutputWithSink(Model& model, const OutputPort<TYPE>& output);                                                                                                                           \
    template const OutputPort<TYPE>& AppendSink(Model& model, const OutputPort<TYPE>& sinkOutput);                                                                                                                                 \
    template const OutputPort<TYPE>& AppendBiasLayer(Model& model, const OutputPort<TYPE>& output, math::ConstColumnVectorReference<TYPE> bias);                                                                                   \
    template const OutputPort<TYPE>& AppendBiasLayer(Model& model, const OutputPort<TYPE>& output, const std::vector<TYPE>& bias);                                                                                                 \
    template const OutputPort<TYPE>& AppendScalingLayer(Model& model, const OutputPort<TYPE>& output, math::ConstColumnVectorReference<TYPE> scale);                                                                               \
    template const OutputPort<TYPE>& AppendScalingLayer(Model& model, const OutputPort<TYPE>& output, const std::vector<TYPE>& scale);                                                                                             \
    template const OutputPort<TYPE>& AppendFullyConnectedLayer(Model& model, const OutputPort<TYPE>& output, math::ConstRowMatrixReference<TYPE> weights);                                                                         \
    template const OutputPort<TYPE>& AppendConvolutionalLayer(Model& model, const OutputPort<TYPE>& output, int filterSize, int stride, int inputPadding, int outputPadding, math::ConstRowMatrixReference<TYPE> weights);         \
    template const OutputPort<TYPE>& AppendFineTunedFullyConnectedNodes(Model& model, const OutputPort<TYPE>& output, const WeightsAndBias<TYPE>& weightsAndBias);                                                                 \
    template const OutputPort<TYPE>& AppendFineTunedConvolutionalNodes(Model& model, const OutputPort<TYPE>& output, int filterSize, int stride, int inputPadding, int outputPadding, const WeightsAndBias<TYPE>& weightsAndBias); \
    template const model::OutputPort<TYPE>& RemoveSourceAndSinkNodes(model::Model& model, const model::OutputPort<TYPE>& output);                                                                                                  \
    template bool IsSourceNode<TYPE>(const Node* node);                                                                                                                                                                            \
    template bool IsSinkNode<TYPE>(const Node* node);                                                                                                                                                                              \
    template bool IsNeuralNetworkPredictorNode<TYPE>(const Node* node);                                                                                                                                                            \
    template bool IsFullyConnectedLayerNode<TYPE>(const Node* node);                                                                                                                                                               \
    template bool IsBiasLayerNode<TYPE>(const Node* node);                                                                                                                                                                         \
    template bool IsConvolutionalLayerNode<TYPE>(const Node* node);

INSTANTIATE_FUNCTIONS(float)
INSTANTIATE_FUNCTIONS(double)

#undef INSTANTIATE_FUNCTIONS
