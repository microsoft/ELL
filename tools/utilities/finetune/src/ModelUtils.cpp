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

#include <nodes/include/ActivationFunctions.h>
#include <nodes/include/ClockNode.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/NeuralNetworkPredictorNode.h>
#include <nodes/include/ReinterpretLayoutNode.h>
#include <nodes/include/ReorderDataNode.h>
#include <nodes/include/SinkNode.h>
#include <nodes/include/SourceNode.h>

namespace ell
{
using namespace ell::model;

namespace
{
    math::TensorShape ToTensorShape(utilities::MemoryShape shape)
    {
        shape.Resize(3);
        return math::TensorShape(shape[0], shape[1], shape[2]);
    }
} // namespace

void SaveModel(const OutputPortBase& output, std::string filename)
{
    ModelTransformer transformer;
    TransformContext context;
    Submodel submodel({ &output });
    auto prunedSubmodel = transformer.CopySubmodel(submodel, context);
    common::SaveModel(prunedSubmodel.GetModel(), filename);
}

// Querying nodes
bool IsInputNode(const model::Node* node)
{
    return dynamic_cast<const InputNodeBase*>(node) != nullptr;
}

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

ConvolutionalNodeType GetConvolutionalNodeType(const Node* node)
{
    if (IsConvolutionalLayerNode<float>(node))
    {
        return GetConvolutionalNodeType<float>(node);
    }
    else if (IsConvolutionalLayerNode<double>(node))
    {
        return GetConvolutionalNodeType<double>(node);
    }
    throw utilities::InputException(utilities::InputExceptionErrors::badData, "GetConvolutionalNodeType called on a non-convolutional layer node");
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

template <typename T>
ConvolutionalNodeType GetConvolutionalNodeType(const Node* node)
{
    auto convNode = static_cast<const nodes::ConvolutionalLayerNode<T>*>(node);
    if (convNode->GetLayer().IsDepthwiseSeparable())
    {
        return ConvolutionalNodeType::spatial;
    }
    else if (convNode->GetLayer().GetConvolutionalParameters().receptiveField == 1)
    {
        return ConvolutionalNodeType::pointwise;
    }
    else
    {
        return ConvolutionalNodeType::full;
    }
}

// Finding things
InputNodeBase* GetInputNode(const model::OutputPortBase& output)
{
    auto model = output.GetNode()->GetModel();
    auto iter = model->GetNodeIterator(&output);
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

// need to visit model via ports, not nodes
InputPortBase* GetInputNodeReference(const OutputPortBase& output)
{
    auto model = output.GetNode()->GetModel();
    auto iter = model->GetNodeIterator(&output);
    while (iter.IsValid())
    {
        auto node = iter.Get();
        auto inputPorts = node->GetInputPorts();
        for (auto inputPort : inputPorts)
        {
            if (IsInputNode(inputPort->GetReferencedPort().GetNode()))
            {
                return inputPort;
            }
        }
        iter.Next();
    }

    throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find input node in model");
}

std::vector<const InputPortBase*> GetInputReferences(const OutputPortBase& input, const OutputPortBase& output)
{
    std::vector<const InputPortBase*> result;
    auto model = input.GetNode()->GetModel()->ShallowCopy();
    auto iter = model.GetNodeIterator(&output);
    while (iter.IsValid())
    {
        auto node = iter.Get();
        auto inputPorts = node->GetInputPorts();
        for (auto inputPort : inputPorts)
        {
            if (&inputPort->GetReferencedPort() == &input)
            {
                result.push_back(inputPort);
            }
        }
        iter.Next();
    }
    return result;
}

int GetInputFanIn(const OutputPortBase& input, const OutputPortBase& output)
{
    int result = 0;
    auto model = input.GetNode()->GetModel()->ShallowCopy();
    auto iter = model.GetNodeIterator(&output);
    while (iter.IsValid())
    {
        auto node = iter.Get();
        auto inputPorts = node->GetInputPorts();
        for (auto inputPort : inputPorts)
        {
            if (&inputPort->GetReferencedPort() == &input)
            {
                ++result;
            }
        }
        iter.Next();
    }
    return result;
}

template <typename ElementType>
InputNode<ElementType>* GetInputNode(const model::OutputPortBase& output)
{
    auto model = output.GetNode()->GetModel();
    auto iter = model->GetNodeIterator(&output);
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
std::vector<const nodes::FullyConnectedLayerNode<ElementType>*> GetFullyConnectedLayerNodes(const OutputPortBase& output)
{
    auto model = output.GetNode()->GetModel();
    std::vector<const nodes::FullyConnectedLayerNode<ElementType>*> result;
    model->VisitSubmodel(&output, [&result](const Node& node) {
        if (IsFullyConnectedLayerNode<ElementType>(&node))
        {
            result.push_back(static_cast<const nodes::FullyConnectedLayerNode<ElementType>*>(&node));
        }
    });
    return result;
}

template <typename ElementType>
const nodes::FullyConnectedLayerNode<ElementType>* GetNearestFullyConnectedLayerNode(const OutputPortBase& output)
{
    const OutputPortBase* port = &output;
    for (;;)
    {
        auto node = port->GetNode();
        if (IsFullyConnectedLayerNode<ElementType>(node))
        {
            return static_cast<const nodes::FullyConnectedLayerNode<ElementType>*>(node);
        }

        if (node->NumInputPorts() != 1)
        {
            // TODO: log error
            return nullptr;
        }
        port = &(node->GetInputPort(0)->GetReferencedPort());
    }
    return nullptr;
}

template <typename ElementType>
std::vector<const nodes::BiasLayerNode<ElementType>*> GetBiasLayerNodes(const OutputPortBase& output)
{
    auto model = output.GetNode()->GetModel();
    std::vector<const nodes::BiasLayerNode<ElementType>*> result;
    model->VisitSubmodel(&output, [&result](const Node& node) {
        if (IsBiasLayerNode<ElementType>(&node))
        {
            result.push_back(static_cast<const nodes::BiasLayerNode<ElementType>*>(&node));
        }
    });
    return result;
}

template <typename ElementType>
std::vector<const nodes::ConvolutionalLayerNode<ElementType>*> GetConvolutionalLayerNodes(const OutputPortBase& output)
{
    auto model = output.GetNode()->GetModel();
    std::vector<const nodes::ConvolutionalLayerNode<ElementType>*> result;
    model->VisitSubmodel(&output, [&result](const Node& node) {
        if (IsConvolutionalLayerNode<ElementType>(&node))
        {
            result.push_back(static_cast<const nodes::ConvolutionalLayerNode<ElementType>*>(&node));
        }
    });
    return result;
}

template <typename ElementType>
const nodes::ConvolutionalLayerNode<ElementType>* GetNearestConvolutionalLayerNode(const OutputPortBase& output)
{
    const OutputPortBase* port = &output;
    for (;;)
    {
        auto node = port->GetNode();
        if (IsConvolutionalLayerNode<ElementType>(node))
        {
            return static_cast<const nodes::ConvolutionalLayerNode<ElementType>*>(node);
        }

        if (node->NumInputPorts() != 1)
        {
            // TODO: log error
            return nullptr;
        }
        port = &(node->GetInputPort(0)->GetReferencedPort());
    }
    return nullptr;
}

template <typename T>
const InputPort<T>& FindFirstConsumer(const OutputPort<T>& port)
{
    auto model = port.GetNode()->GetModel();
    const OutputPortBase* portBase = static_cast<const OutputPortBase*>(&port);
    auto it = model->GetNodeIterator();
    while (it.IsValid())
    {
        auto node = it.Get();
        for (auto dependent : node->GetDependentNodes())
        {
            for (auto dependentPort : dependent->GetInputPorts())
            {
                if (&dependentPort->GetReferencedPort() == portBase)
                {
                    return static_cast<const InputPort<T>&>(*dependentPort);
                }
            }
        }
        it.Next();
    }

    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
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

// TODO: reverse order of args?
bool IsCompleteAncestor(const OutputPortBase& p, const OutputPortBase& a)
{
    // returns true iff a is on every path from p to start
    if (&p == &a)
    {
        return true;
    }
    auto node = p.GetNode();
    if (node->NumInputPorts() == 0)
    {
        return false;
    }
    for (auto inPort : node->GetInputPorts())
    {
        if (!IsCompleteAncestor(inPort->GetReferencedPort(), a))
        {
            return false;
        }
    }
    return true;
}

void GetCompleteAncestorsImpl(const OutputPortBase& p, const OutputPortBase& a, std::unordered_set<const OutputPortBase*>& ancestors);

std::unordered_set<const OutputPortBase*> GetCompleteAncestors(const OutputPortBase& p)
{
    std::unordered_set<const OutputPortBase*> ancestors = { &p };
    GetCompleteAncestorsImpl(p, p, ancestors);
    return ancestors;
}

// TODO: combine this and IsCompleteAncestor so it's not O(N^2) (via set-intersect?)
void GetCompleteAncestorsImpl(const OutputPortBase& p, const OutputPortBase& a, std::unordered_set<const OutputPortBase*>& ancestors)
{
    auto node = a.GetNode();
    for (auto inPort : node->GetInputPorts())
    {
        const auto& outPort = inPort->GetReferencedPort();
        if (IsCompleteAncestor(p, outPort))
        {
            ancestors.insert(&outPort);
        }
        GetCompleteAncestorsImpl(p, outPort, ancestors);
    }
}

const OutputPortBase* FindNearestCommonAncestor(const OutputPortBase& a, const OutputPortBase& b)
{
    // In this graph:
    //     |
    //    (A)
    //     |
    //    (B)
    //   /   \ 
    // (C)   (D)
    //  |     |
    //  |    (E)
    //   \   /
    //    (F)
    //     |
    //    (G)
    //
    // the nearest common ancestor to (C) and (E) is (B)
    // the nearest common ancestor to (E) and (F) is (B)
    // the nearest common ancestor to (F) and (G) is (F)
    // the nearest common ancestor to (D) and (E) is (D)

    //    (A)
    //     |
    //    (B)
    //   /   \ 
    // (C)   (D)
    //   \   /
    //    (E)
    //     |
    //    (F)
    //   /   \ 
    // (G)   (H)
    //   \   /
    //    (I)

    throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "FindNearestCommonAncestor not implemented");
}

const OutputPortBase* FindNearestCommonAncestor(std::vector<const OutputPortBase*> ports)
{
    if (ports.size() == 0)
    {
        return nullptr;
    }
    else if (ports.size() == 1)
    {
        return ports[0];
    }
    else
    {
        auto p1 = ports.back();
        ports.pop_back();
        auto p2 = ports.back();
        ports.pop_back();
        auto nearest = FindNearestCommonAncestor(*p1, *p2);
        if (nearest == nullptr)
        {
            return nearest;
        }
        else
        {
            ports.push_back(nearest);
            return FindNearestCommonAncestor(ports);
        }
    }
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

Map GetMapPrefix(Map& map, const std::string& outputElementsString)
{
    Model& model = map.GetModel();
    auto parsedOutputElements = ParsePortElementsProxy(outputElementsString);
    auto outputElements = ProxyToPortElements(model, parsedOutputElements);
    const auto& output = model.SimplifyOutputs(outputElements);
    const auto& input = map.GetInput();
    return Map(model, { { "input", input } }, { { "output", output } });
}

//
// Model-related
//
template <typename ElementType>
const OutputPort<ElementType>& AppendOutputWithSink(const OutputPort<ElementType>& output)
{
    Model& model = *(output.GetNode()->GetModel());
    const auto& sinkOutput = AppendSink<ElementType>(output);
    auto outputNode = model.AddNode<OutputNode<ElementType>>(sinkOutput);
    return outputNode->output;
}

template <typename ElementType>
const OutputPort<ElementType>& AppendSink(const OutputPort<ElementType>& output)
{
    Model& model = *(output.GetNode()->GetModel());
    auto condition = model.AddNode<nodes::ConstantNode<bool>>(true);
    auto sinkNode = model.AddNode<nodes::SinkNode<ElementType>>(output, condition->output, "OutputCallback");
    return sinkNode->output;
}

template <typename ElementType>
const OutputPort<ElementType>& Bias(const OutputPort<ElementType>& output, math::ConstColumnVectorReference<ElementType> bias)
{
    return Bias(output, bias.ToArray());
}

template <typename ElementType>
const OutputPort<ElementType>& Bias(const OutputPort<ElementType>& output, const std::vector<ElementType>& bias)
{
    Model& model = *(output.GetNode()->GetModel());
#if 1
    // Version from master branch
    auto shape = ToTensorShape(output.GetMemoryLayout().GetActiveSize());
    typename predictors::neural::Layer<ElementType>::TensorType inputTensorPlaceholder(shape);
    typename predictors::neural::Layer<ElementType>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<ElementType>::LayerParameters biasParams{ inputTensorPlaceholderRef, predictors::neural::NoPadding(), shape, predictors::neural::NoPadding() };
    auto biasVector = math::ColumnVector<ElementType>(bias);
    auto biasRef = biasVector.GetConstReference();
    predictors::neural::BiasLayer<ElementType> biasLayer(biasParams, biasRef);
    auto biasNode = model.AddNode<nodes::BiasLayerNode<ElementType>>(output, biasLayer);
    return biasNode->output;

#else

    auto memoryLayout = output.GetMemoryLayout();
    auto activeLayout = PortMemoryLayout{ memoryLayout.GetActiveSize() };
    // auto vectorLayout = PortMemoryLayout({ memoryLayout.NumElements() });

    // strip padding and reorder to vector
    const auto& unpaddedData = nodes::ReinterpretLayout(output, activeLayout);
    // const auto& vectorData = nodes::ReinterpretLayout(unpaddedData, vectorLayout);

    auto scaleValuesNode = model.AddNode<nodes::ConstantNode<ElementType>>(); // nothing
    auto biasValuesNode = model.AddNode<nodes::ConstantNode<ElementType>>(bias);

    const size_t channelDimension = 2;
    // auto computeNode = model.AddNode<nodes::BroadcastLinearFunctionNode<ElementType>>(vectorData,
    //                                                                                   vectorLayout,
    auto computeNode = model.AddNode<nodes::BroadcastLinearFunctionNode<ElementType>>(unpaddedData,
                                                                                      activeLayout,
                                                                                      scaleValuesNode->output,
                                                                                      biasValuesNode->output,
                                                                                      channelDimension,
                                                                                      activeLayout);
    const auto& restoredData1 = nodes::ReinterpretLayout(computeNode->output, memoryLayout);
    return restoredData1;
// const auto& restoredData1 = nodes::ReinterpretLayout(model, computeNode->output, activeLayout);
// const auto& restoredData2 = nodes::ReinterpretLayout(model, restoredData1, memoryLayout);
// return restoredData2;
#endif
}

template <typename ElementType>
const OutputPort<ElementType>& Scale(const OutputPort<ElementType>& output, math::ConstColumnVectorReference<ElementType> scale)
{
    return Scale(output, scale.ToArray());
}

template <typename ElementType>
const OutputPort<ElementType>& Scale(const OutputPort<ElementType>& output, const std::vector<ElementType>& scale)
{
    Model& model = *(output.GetNode()->GetModel());
#if 1
    // Version from master branch
    auto inputShape = output.GetMemoryLayout().GetActiveSize();
    typename predictors::neural::Layer<ElementType>::TensorType inputTensorPlaceholder(ToTensorShape(inputShape));
    typename predictors::neural::Layer<ElementType>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<ElementType>::LayerParameters scaleParams{ inputTensorPlaceholderRef, predictors::neural::NoPadding(), ToTensorShape(inputShape), predictors::neural::NoPadding() };
    auto scaleVector = math::ColumnVector<ElementType>(scale);
    auto scaleRef = scaleVector.GetConstReference();
    predictors::neural::ScalingLayer<ElementType> scaleLayer(scaleParams, scaleRef);
    auto scaleNode = model.AddNode<nodes::ScalingLayerNode<ElementType>>(output, scaleLayer);
    return scaleNode->output;
#else
    auto memoryLayout = output.GetMemoryLayout();
    auto activeLayout = PortMemoryLayout{ memoryLayout.GetActiveSize() };
    auto vectorLayout = PortMemoryLayout({ memoryLayout.NumElements() });

    // strip padding and reorder to vector
    const auto& unpaddedData = nodes::ReinterpretLayout(output, activeLayout);
    const auto& vectorData = nodes::ReinterpretLayout(unpaddedData, vectorLayout);

    auto biasValuesNode = model.AddNode<nodes::ConstantNode<ElementType>>(); // nothing
    auto scaleValuesNode = model.AddNode<nodes::ConstantNode<ElementType>>(scale);

    const size_t channelDimension = 2;
    auto computeNode = model.AddNode<nodes::BroadcastLinearFunctionNode<ElementType>>(vectorData,
                                                                                      vectorLayout,
                                                                                      scaleValuesNode->output,
                                                                                      biasValuesNode->output,
                                                                                      channelDimension,
                                                                                      vectorLayout);
    const auto& restoredData1 = nodes::ReinterpretLayout(computeNode->output, activeLayout);
    const auto& restoredData2 = nodes::ReinterpretLayout(restoredData1, memoryLayout);
    return restoredData2;
#endif
}

template <typename ElementType>
const OutputPort<ElementType>& AppendFullyConnectedLayer(const OutputPort<ElementType>& output, math::ConstRowMatrixReference<ElementType> weights)
{
    Model& model = *(output.GetNode()->GetModel());
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
const OutputPort<ElementType>& AppendConvolutionalLayer(const OutputPort<ElementType>& output, const ConvolutionalParameters& convParams, math::ConstRowMatrixReference<ElementType> weights)
{
    Model& model = *(output.GetNode()->GetModel());
    const auto inputShape = output.GetMemoryLayout().GetExtent();
    const auto numRows = inputShape[0];
    const auto numColumns = inputShape[1];
    const auto numChannels = inputShape[2];

    const auto numOutputRows = (numRows - convParams.filterSize + 1) / convParams.stride;
    const auto numOutputColumns = (numColumns - convParams.filterSize + 1) / convParams.stride;
    const auto numOutputChannels = weights.NumRows();
    auto numWeightsChannels = convParams.isDepthwiseSeparable ? 1 : numChannels;

    // TODO: pass in original layer's output shape, for verification? (or, compare output of this function with original layer output shape)

    typename predictors::neural::Layer<ElementType>::TensorType inputTensorPlaceholder(ToTensorShape(inputShape));
    typename predictors::neural::Layer<ElementType>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<ElementType>::LayerParameters layerParams{ inputTensorPlaceholderRef, predictors::neural::ZeroPadding(convParams.inputPadding), { static_cast<size_t>(numOutputRows), static_cast<size_t>(numOutputColumns), static_cast<size_t>(numOutputChannels) }, predictors::neural::ZeroPadding(convParams.outputPadding) };
    predictors::neural::ConvolutionalParameters convLayerParams{ static_cast<size_t>(convParams.filterSize), static_cast<size_t>(convParams.stride), predictors::neural::ConvolutionMethod::automatic, 1 };
    math::ChannelColumnRowTensor<ElementType> weightsTensor(static_cast<size_t>(numOutputChannels * convParams.filterSize), static_cast<size_t>(convParams.filterSize), numWeightsChannels, weights.ToArray());
    predictors::neural::ConvolutionalLayer<ElementType> convLayer(layerParams, convLayerParams, weightsTensor);
    auto convNode = model.AddNode<nodes::ConvolutionalLayerNode<ElementType>>(output, convLayer);
    return convNode->output;
}

template <typename ElementType>
const OutputPort<ElementType>& Normalize(const OutputPort<ElementType>& output, const DataStatistics& stats)
{
    auto size = stats.mean.Size();
    if (stats.stdDev.Size() != size)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Before and after stats sizes don't match");
    }
    auto negativeBias = std::vector<ElementType>(size);
    auto inverseStd = std::vector<ElementType>(size);
    for (size_t i = 0; i < size; ++i)
    {
        negativeBias[i] = static_cast<ElementType>(-stats.mean[i]);
        inverseStd[i] = 1.0f / static_cast<ElementType>(stats.stdDev[i]);
    }

    const auto& meanSubtractOutput = Bias(output, negativeBias);
    const auto& invertVarianceOutput = Scale(meanSubtractOutput, inverseStd);
    return invertVarianceOutput;
}

template <typename ElementType>
const OutputPort<ElementType>& Unnormalize(const OutputPort<ElementType>& output, const DataStatistics& originalOutputStats, const DataStatistics& fineTunedOutputStats)
{
    auto size = originalOutputStats.mean.Size();
    if (originalOutputStats.stdDev.Size() != size || fineTunedOutputStats.mean.Size() != size || fineTunedOutputStats.stdDev.Size() != size)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Before and after stats sizes don't match");
    }
    auto stdDev = std::vector<ElementType>(size);
    auto bias = std::vector<ElementType>(size);
    for (size_t i = 0; i < size; ++i)
    {
        stdDev[i] = static_cast<ElementType>(originalOutputStats.stdDev[i] / fineTunedOutputStats.stdDev[i]);
        bias[i] = static_cast<ElementType>(originalOutputStats.mean[i] - (fineTunedOutputStats.mean[i] * stdDev[i]));
    }

    const auto& restoreVarianceOutput = Scale(output, stdDev);
    const auto& restoreMeanOutput = Bias(restoreVarianceOutput, bias);
    return restoreMeanOutput;
}

// TODO: rename 'output' parameter -- it's where we graft the new node
template <typename ElementType>
const OutputPort<ElementType>& AppendFineTunedNodes(const OutputPort<ElementType>& output, const FullyConnectedParameters& fcParams, const WeightsAndBias<ElementType>& weightsAndBias)
{
    auto weightsRef = weightsAndBias.weights.GetConstReference();
    auto biasRef = weightsAndBias.bias.GetConstReference();
    const auto& fc = AppendFullyConnectedLayer(output, weightsRef);
    const auto& bias = Bias(fc, biasRef);
    return bias;
}

// TODO: rename 'output' parameter -- it's where we graft the new node
template <typename ElementType>
const OutputPort<ElementType>& AppendFineTunedNodes(const OutputPort<ElementType>& output, const ConvolutionalParameters& convParams, const WeightsAndBias<ElementType>& weightsAndBias)
{
    auto weightsRef = weightsAndBias.weights.GetConstReference();
    auto biasRef = weightsAndBias.bias.GetConstReference();
    const auto& conv = AppendConvolutionalLayer(output, convParams, weightsRef);
    const auto& bias = Bias(conv, biasRef);
    return bias;
}

void ReplaceSourceNode(ModelTransformer& transformer, const model::SourceNodeBase& n);
void ReplaceSinkNode(ModelTransformer& transformer, const model::SinkNodeBase& n);
const model::OutputPortBase& RemoveSourceAndSinkNodes(const model::OutputPortBase& output);

template <typename ElementType>
const model::OutputPort<ElementType>& RemoveSourceAndSinkNodes(const model::OutputPort<ElementType>& output)
{
    const auto& result = RemoveSourceAndSinkNodes(static_cast<const model::OutputPortBase&>(output));
    return static_cast<const model::OutputPort<ElementType>&>(result);
}

const model::OutputPortBase& RemoveSourceAndSinkNodes(const model::OutputPortBase& output)
{
    TransformContext context;
    ModelTransformer transformer;
    Submodel submodel({ &output });
    auto newSubmodel = transformer.TransformSubmodel(submodel, context, [](const Node& n, ModelTransformer& transformer) {
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

#define INSTANTIATE_FUNCTIONS(TYPE)                                                                                                                                                    \
    template InputNode<TYPE>* GetInputNode(const OutputPortBase& output);                                                                                                              \
    template const OutputNode<TYPE>* GetOutputNode(const Model& model);                                                                                                                \
    template std::vector<const nodes::FullyConnectedLayerNode<TYPE>*> GetFullyConnectedLayerNodes(const OutputPortBase& output);                                                       \
    template const nodes::FullyConnectedLayerNode<TYPE>* GetNearestFullyConnectedLayerNode(const OutputPortBase& output);                                                              \
    template std::vector<const nodes::BiasLayerNode<TYPE>*> GetBiasLayerNodes(const OutputPortBase& output);                                                                           \
    template std::vector<const nodes::ConvolutionalLayerNode<TYPE>*> GetConvolutionalLayerNodes(const OutputPortBase& output);                                                         \
    template const nodes::ConvolutionalLayerNode<TYPE>* GetNearestConvolutionalLayerNode(const OutputPortBase& output);                                                                \
    template const OutputPort<TYPE>& AppendOutputWithSink(const OutputPort<TYPE>& output);                                                                                             \
    template const OutputPort<TYPE>& AppendSink(const OutputPort<TYPE>& sinkOutput);                                                                                                   \
    template const OutputPort<TYPE>& Bias(const OutputPort<TYPE>& output, math::ConstColumnVectorReference<TYPE> bias);                                                                \
    template const OutputPort<TYPE>& Bias(const OutputPort<TYPE>& output, const std::vector<TYPE>& bias);                                                                              \
    template const OutputPort<TYPE>& Scale(const OutputPort<TYPE>& output, math::ConstColumnVectorReference<TYPE> scale);                                                              \
    template const OutputPort<TYPE>& Scale(const OutputPort<TYPE>& output, const std::vector<TYPE>& scale);                                                                            \
    template const OutputPort<TYPE>& AppendFullyConnectedLayer(const OutputPort<TYPE>& output, math::ConstRowMatrixReference<TYPE> weights);                                           \
    template const OutputPort<TYPE>& AppendConvolutionalLayer(const OutputPort<TYPE>& output, const ConvolutionalParameters& convParams, math::ConstRowMatrixReference<TYPE> weights); \
    template const OutputPort<TYPE>& Normalize(const OutputPort<TYPE>& output, const DataStatistics& stats);                                                                           \
    template const OutputPort<TYPE>& Unnormalize(const OutputPort<TYPE>& output, const DataStatistics& originalOutputStats, const DataStatistics& fineTunedOutputStats);               \
    template const OutputPort<TYPE>& AppendFineTunedNodes(const OutputPort<TYPE>& output, const FullyConnectedParameters& fcParams, const WeightsAndBias<TYPE>& weightsAndBias);       \
    template const OutputPort<TYPE>& AppendFineTunedNodes(const OutputPort<TYPE>& output, const ConvolutionalParameters& convParams, const WeightsAndBias<TYPE>& weightsAndBias);      \
    template const model::OutputPort<TYPE>& RemoveSourceAndSinkNodes(const model::OutputPort<TYPE>& output);                                                                           \
    template bool IsSourceNode<TYPE>(const Node* node);                                                                                                                                \
    template bool IsSinkNode<TYPE>(const Node* node);                                                                                                                                  \
    template bool IsNeuralNetworkPredictorNode<TYPE>(const Node* node);                                                                                                                \
    template bool IsFullyConnectedLayerNode<TYPE>(const Node* node);                                                                                                                   \
    template bool IsBiasLayerNode<TYPE>(const Node* node);                                                                                                                             \
    template bool IsConvolutionalLayerNode<TYPE>(const Node* node);                                                                                                                    \
    template const InputPort<TYPE>& FindFirstConsumer(const OutputPort<TYPE>& port);

INSTANTIATE_FUNCTIONS(float)
INSTANTIATE_FUNCTIONS(double)

#undef INSTANTIATE_FUNCTIONS
} // namespace ell
