////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelInterface.h"
#include "DatasetInterface.h"
#include "DatasetInterfaceImpl.h"
#include "ModelBuilderInterface.h"

#include <common/include/LoadModel.h>
#include <common/include/MapLoadArguments.h>
#include <common/include/RegisterNodeCreators.h>

#include <data/include/DenseDataVector.h>

#include <dsp/include/FilterBank.h>

#include <emitters/include/ModuleEmitter.h>

#include <model/include/InputNode.h>
#include <model/include/Map.h>
#include <model/include/OutputNode.h>

#include <utilities/include/JsonArchiver.h>
#include <utilities/include/StringUtil.h>
#include <utilities/include/UniqueNameList.h>

#include <algorithm>

using namespace ell::utilities;

namespace ELL_API
{

//
// NodeIterator
//
bool NodeIterator::IsValid()
{
    if (_isVector)
    {
        return _i < _nodes.size();
    }
    else
    {
        return _iterator.IsValid();
    }
}

void NodeIterator::Next()
{
    if (_isVector)
    {
        _i = _i + 1;
    }
    else
    {
        _iterator.Next();
    }
}

Node NodeIterator::Get()
{
    if (_isVector)
    {
        if (_i >= _nodes.size())
        {
            throw std::out_of_range("invalid iterator");
        }
        return Node(_nodes[_i], _model);
    }
    else
    {
        return Node(_iterator.Get(), _model);
    }
}

NodeIterator::NodeIterator(const std::vector<const ell::model::Node*>& nodes, std::shared_ptr<ell::model::Model> model) :
    _i(0),
    _isVector(true),
    _nodes(nodes),
    _iterator(),
    _model(model)
{
}

NodeIterator::NodeIterator(ell::model::ForwardNodeIterator& other, std::shared_ptr<ell::model::Model> model) :
    _i(0),
    _isVector(false),
    _nodes(0),
    _iterator(other),
    _model(model)
{
}

//
// Node
//
Node::Node(const ell::model::Node* other, std::shared_ptr<ell::model::Model> model) :
    _node(other),
    _model(model)
{
}

std::string Node::GetId()
{
    return to_string(_node->GetId());
}

Model Node::GetModel()
{
    return Model(_model);
}

NodeIterator Node::GetParents()
{
    return NodeIterator(_node->GetParentNodes(), _model);
}

NodeIterator Node::GetDependents()
{
    return NodeIterator(_node->GetDependentNodes(), _model);
}

OutputPort Node::GetOutputPort(const std::string& portName)
{
    auto port = _node->GetOutputPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '" + portName + "'");
    }
    return OutputPort(port, _model);
}

InputPort Node::GetInputPort(const std::string& portName)
{
    using namespace std::string_literals;
    auto port = _node->GetInputPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '"s + portName + "'");
    }
    return InputPort(port, _model);
}

Port Node::GetPort(const std::string& portName)
{
    using namespace std::string_literals;
    auto port = _node->GetPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '"s + portName + "'");
    }
    return Port(port, _model);
}

OutputPortIterator Node::GetOutputPorts()
{
    auto ports = _node->GetOutputPorts();
    std::vector<const ell::model::OutputPortBase*> constPorts(ports.begin(), ports.end());
    return OutputPortIterator(constPorts, _model);
}

InputPortIterator Node::GetInputPorts()
{
    auto ports = _node->GetInputPorts();
    std::vector<const ell::model::InputPortBase*> constPorts(ports.begin(), ports.end());
    return InputPortIterator(constPorts, _model);
}

std::string Node::GetRuntimeTypeName()
{
    return _node->GetRuntimeTypeName();
}

std::string Node::GetMetadataValue(const std::string& key)
{
    std::string value;
    if (_node->GetMetadata().HasEntry(key))
    {
        value = _node->GetMetadata().GetEntry<std::string>(key);
    }
    return value;
}

void Node::SetMetadataValue(const std::string& key, const std::string& value)
{
    auto node = const_cast<ell::model::Node*>(_node);
    node->GetMetadata()[key] = value;
}

void Node::CopyMetadataFrom(const Node& other)
{
    const_cast<ell::model::Node*>(_node)->GetMetadata() = other._node->GetMetadata();
}

//
// InputNode
//
InputNode::InputNode(const InputNode& node, std::shared_ptr<ell::model::Model> model) :
    Node(node.GetNode(), model)
{
    if (dynamic_cast<const ell::model::InputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an InputNode");
    }
}

InputNode::InputNode(Node node) :
    Node(node.GetNode(), node.GetModel().GetModel())
{
    if (dynamic_cast<const ell::model::InputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an InputNode");
    }
}

InputNode::InputNode(const ell::model::InputNodeBase* other, std::shared_ptr<ell::model::Model> model) :
    Node(other, model)
{
}

const ell::model::InputNodeBase* InputNode::GetInputNode() const
{
    return dynamic_cast<const ell::model::InputNodeBase*>(GetNode());
}

//
// OutputNode
//
OutputNode::OutputNode(const OutputNode& node, std::shared_ptr<ell::model::Model> model) :
    Node(node.GetNode(), model)
{
    if (GetOutputNode() == nullptr)
    {
        throw std::invalid_argument("Error: not an OutputNode");
    }
}

OutputNode::OutputNode(Node node) :
    Node(node.GetNode(), node.GetModel().GetModel())
{
    if (GetOutputNode() == nullptr)
    {
        throw std::invalid_argument("Error: not an OutputNode");
    }
}

OutputNode::OutputNode(const ell::model::OutputNodeBase* other, std::shared_ptr<ell::model::Model> model) :
    Node(other, model)
{
}

const ell::model::OutputNodeBase* OutputNode::GetOutputNode() const
{
    return dynamic_cast<const ell::model::OutputNodeBase*>(GetNode());
}

//
// SourceNode
//
SourceNode::SourceNode(const SourceNode& node, std::shared_ptr<ell::model::Model> model) :
    Node(node.GetNode(), model),
    _sourceNode(node._sourceNode)
{
}

SourceNode::SourceNode(ell::model::SourceNodeBase* other, std::shared_ptr<ell::model::Model> model) :
    Node(other, model),
    _sourceNode(other)
{
}

ell::model::SourceNodeBase* SourceNode::GetSourceNode() const
{
    return _sourceNode;
}

//
// SinkNode
//
SinkNode::SinkNode(const SinkNode& node, std::shared_ptr<ell::model::Model> model) :
    Node(node.GetNode(), model),
    _sinkNode(node._sinkNode)
{
}

SinkNode::SinkNode(ell::model::SinkNodeBase* other, std::shared_ptr<ell::model::Model> model) :
    Node(other, model),
    _sinkNode(other)
{
}

ell::model::SinkNodeBase* SinkNode::GetSinkNode() const
{
    return _sinkNode;
}

//
// Model
//

Model::Model()
{
    _model = std::make_shared<ell::model::Model>();
}
Model::Model(const std::string& filename)
{
    Load(filename);
}

Model::Model(std::shared_ptr<ell::model::Model> underlyingModel) :
    _model(underlyingModel)
{
}

void Model::Load(const std::string& filename)
{
    _model = std::make_shared<ell::model::Model>(ell::common::LoadModel(filename));
}

void Model::Save(const std::string& filename)
{
    ell::common::SaveModel(*_model, filename);
}

void Model::LoadFromString(const std::string& str)
{
    _model = std::make_shared<ell::model::Model>();
    std::stringstream stream(str);
    SerializationContext context;
    JsonUnarchiver ar(stream, context);
    ar >> *_model;
}

size_t Model::Size()
{
    return _model->Size();
}

NodeIterator Model::GetNodes()
{
    auto iter = _model->GetNodeIterator();
    return NodeIterator(iter, _model);
}

std::string Model::GetJson() const
{
    std::stringstream stream;
    JsonArchiver ar(stream);
    ar << *_model;
    return stream.str();
}

ModelBuilder builder;

PortElements GetDefaultOutput(Node node)
{
    OutputPortIterator iter = node.GetOutputPorts();
    if (iter.IsValid())
    {
        auto port = iter.Get();
        return PortElements(port);
    }
    else
    {
        throw std::out_of_range("node has no output port");
    }
}

// This provides a simpler version of ModelBuilder that hides Ports
Node Model::AddBinaryOperation(Node input1, Node input2, BinaryOperationType operation)
{
    return builder.AddBinaryOperationNode(Model(_model), GetDefaultOutput(input1), GetDefaultOutput(input2), operation);
}

Node Model::AddBuffer(Node input, int windowSize)
{
    return builder.AddBufferNode(Model(_model), GetDefaultOutput(input), windowSize);
}

Node Model::AddClock(Node input, double interval, double lagThreshold, const std::string& lagNotificationName)
{
    return builder.AddClockNode(Model(_model), GetDefaultOutput(input), interval, lagThreshold, lagNotificationName);
}

Node Model::AddConcatenation(const PortMemoryLayout& outputMemoryLayout, const std::vector<Node>& inputs)
{
    std::vector<PortElements> elements_list; // keep the object alive.
    std::vector<PortElements*> elements;
    for (const auto node : inputs)
    {
        elements_list.push_back(GetDefaultOutput(node));
        elements.push_back(&elements_list.back());
    }
    return builder.AddConcatenationNode(Model(_model), outputMemoryLayout, elements);
}

Node Model::AddConstant(std::vector<double> values, const PortMemoryLayout& outputMemoryLayout, PortType type)
{
    return builder.AddConstantNode(Model(_model), values, outputMemoryLayout, type);
}

Node Model::AddConstant(std::vector<double> values, PortType type)
{
    return builder.AddConstantNode(Model(_model), values, type);
}

Node Model::AddDCT(Node input, int numFilters)
{
    return builder.AddDCTNode(Model(_model), GetDefaultOutput(input), numFilters);
}

Node Model::AddDotProduct(Node input1, Node input2)
{
    return builder.AddDotProductNode(Model(_model), GetDefaultOutput(input1), GetDefaultOutput(input2));
}

Node Model::AddDTW(std::vector<std::vector<double>> prototype, Node input)
{
    return builder.AddDTWNode(Model(_model), prototype, GetDefaultOutput(input));
}

Node Model::AddFFT(Node input, int nfft)
{
    return builder.AddFFTNode(Model(_model), GetDefaultOutput(input), nfft);
}

Node Model::AddGRU(Node input, Node reset, size_t hiddenUnits, Node inputWeights, Node hiddenWeights, Node inputBias, Node hiddenBias, ell::api::predictors::neural::ActivationType activation, ell::api::predictors::neural::ActivationType recurrentActivation)
{
    return builder.AddGRUNode(Model(_model), GetDefaultOutput(input), GetDefaultOutput(reset), hiddenUnits, GetDefaultOutput(inputWeights), GetDefaultOutput(hiddenWeights), GetDefaultOutput(inputBias), GetDefaultOutput(hiddenBias), activation, recurrentActivation);
}

Node Model::AddHammingWindow(Node input)
{
    return builder.AddHammingWindowNode(Model(_model), GetDefaultOutput(input));
}

Node Model::AddIIRFilter(Node input, std::vector<double> bCoeffs, std::vector<double> aCoeffs)
{
    return builder.AddIIRFilterNode(Model(_model), GetDefaultOutput(input), bCoeffs, aCoeffs);
}

InputNode Model::AddInput(const PortMemoryLayout& memoryLayout, PortType type)
{
    return builder.AddInputNode(Model(_model), memoryLayout, type);
}

Node Model::AddLinearFilterBank(Node input, double sampleRate, int numFilters, int numFiltersToUse, double offset)
{
    return builder.AddLinearFilterBankNode(Model(_model), GetDefaultOutput(input), sampleRate, numFilters, numFiltersToUse, offset);
}

Node Model::AddLSTM(Node input, Node reset, size_t hiddenUnits, Node inputWeights, Node hiddenWeights, Node inputBias, Node hiddenBias, ell::api::predictors::neural::ActivationType activation, ell::api::predictors::neural::ActivationType recurrentActivation)
{
    return builder.AddLSTMNode(Model(_model), GetDefaultOutput(input), GetDefaultOutput(reset), hiddenUnits, GetDefaultOutput(inputWeights), GetDefaultOutput(hiddenWeights), GetDefaultOutput(inputBias), GetDefaultOutput(hiddenBias), activation, recurrentActivation);
}

Node Model::AddMatrixMultiply(Node input1, Node input2)
{
    return builder.AddMatrixMultiplyNode(Model(_model), GetDefaultOutput(input1), GetDefaultOutput(input2));
}

Node Model::AddMelFilterBank(Node input, double sampleRate, int fftSize, int numFilters, int numFiltersToUse, double offset)
{
    return builder.AddMelFilterBankNode(Model(_model), GetDefaultOutput(input), sampleRate, fftSize, numFilters, numFiltersToUse, offset);
}

Node Model::AddNeuralNetworkPredictor(Node input, ell::api::predictors::NeuralNetworkPredictor predictor)
{
    return builder.AddNeuralNetworkPredictorNode(Model(_model), GetDefaultOutput(input), predictor);
}

OutputNode Model::AddOutput(const PortMemoryLayout& memoryLayout, Node input)
{
    return builder.AddOutputNode(Model(_model), memoryLayout, GetDefaultOutput(input));
}

Node Model::AddReinterpretLayout(Node input, PortMemoryLayout outputMemoryLayout)
{
    return builder.AddReinterpretLayoutNode(Model(_model), GetDefaultOutput(input), outputMemoryLayout);
}

Node Model::AddReorderData(Node input, PortMemoryLayout inputMemoryLayout, PortMemoryLayout outputMemoryLayout, std::vector<int> order, double outputPaddingValue)
{
    return builder.AddReorderDataNode(Model(_model), GetDefaultOutput(input), inputMemoryLayout, outputMemoryLayout, order, outputPaddingValue);
}

Node Model::AddRNN(Node input, Node reset, size_t hiddenUnits, Node inputWeights, Node hiddenWeights, Node inputBias, Node hiddenBias, ell::api::predictors::neural::ActivationType activation)
{
    return builder.AddRNNNode(Model(_model), GetDefaultOutput(input), GetDefaultOutput(reset), hiddenUnits, GetDefaultOutput(inputWeights), GetDefaultOutput(hiddenWeights), GetDefaultOutput(inputBias), GetDefaultOutput(hiddenBias), activation);
}

SinkNode Model::AddSink(Node input, const PortMemoryLayout& memoryLayout, const std::string& sinkFunctionName, Node trigger)
{
    PortElements triggerElements;
    if (trigger.GetNode() != nullptr)
    {
        triggerElements = GetDefaultOutput(trigger);
    }
    return builder.AddSinkNode(Model(_model), GetDefaultOutput(input), memoryLayout, sinkFunctionName, triggerElements);
}

SourceNode Model::AddSource(Node input, PortType outputType, const PortMemoryLayout& memoryLayout, const std::string& sourceFunctionName)
{
    return builder.AddSourceNode(Model(_model), GetDefaultOutput(input), outputType, memoryLayout, sourceFunctionName);
}

Node Model::AddSplice(const std::vector<Node>& inputs)
{
    std::vector<PortElements> elements_list; // keep them alive
    std::vector<PortElements*> elements;
    for (const auto node : inputs)
    {
        elements_list.push_back(GetDefaultOutput(node));
        elements.push_back(&elements_list.back());
    }
    return builder.AddSpliceNode(Model(_model), elements);
}

Node Model::AddTypeCast(Node input, PortType outputType)
{
    return builder.AddTypeCastNode(Model(_model), GetDefaultOutput(input), outputType);
}

Node Model::AddUnaryOperation(Node input, UnaryOperationType operation)
{
    return builder.AddUnaryOperationNode(Model(_model), GetDefaultOutput(input), operation);
}

Node Model::AddVoiceActivityDetector(Node input, double sampleRate, double frameDuration, double tauUp, double tauDown, double largeInput, double gainAtt, double thresholdUp, double thresholdDown, double levelThreshold)
{
    return builder.AddVoiceActivityDetectorNode(Model(_model), GetDefaultOutput(input), sampleRate, frameDuration, tauUp, tauDown, largeInput, gainAtt, thresholdUp, thresholdDown, levelThreshold);
}

std::string Model::GetMetadataValue(const std::string& key)
{
    std::string value;
    if (_model->GetMetadata().HasEntry(key))
    {
        value = _model->GetMetadata().GetEntry<std::string>(key);
    }
    return value;
}

void Model::SetMetadataValue(const std::string& key, const std::string& value)
{
    _model->GetMetadata()[key] = value;
}

Model::Model(ell::model::Model&& other)
{
    _model = std::make_shared<ell::model::Model>(std::move(other));
}

std::shared_ptr<ell::model::Model> Model::GetModel() const
{
    return _model;
}

//
// Map
//
Map::Map()
{
    _map = std::make_shared<ell::model::Map>();
}

std::string GetVariableName(const ell::model::PortElementsBase& portElements, const std::string& defaultName)
{
    // a PortElements object can pull output from multiple nodes, so the friendly name should then be
    // the concatenation of friendly names of each of those output nodes.
    std::vector<std::string> parts;
    for (auto range : portElements.GetRanges())
    {
        const ell::model::OutputPortBase* outputPort = range.ReferencedPort();
        std::string name = outputPort->GetVariableName(defaultName);
        parts.push_back(name);
    }
    if (parts.size() == 0)
    {
        return defaultName;
    }
    return ell::utilities::Join(parts, "_");
}

Map::Map(Model model, InputNode inputNode, PortElements output)
{
    const ell::model::InputNodeBase* innerInputNode = inputNode.GetInputNode();
    std::string name = innerInputNode->GetFriendlyName();
    if (name.empty())
    {
        name = "input";
    }

    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> inputs = { std::pair<std::string, ell::model::InputNodeBase*>{ name, const_cast<ell::model::InputNodeBase*>(innerInputNode) } };

    const ell::model::PortElementsBase& innerPortElements = output.GetPortElements();
    name = GetVariableName(innerPortElements, "output");
    std::shared_ptr<ell::model::Model> ellModel = model.GetModel();
    auto outputs = std::vector<std::pair<std::string, const ell::model::OutputPortBase&>>{ { name, ellModel->SimplifyOutputs(innerPortElements) } };
    _map = std::make_shared<ell::model::Map>(*ellModel, inputs, outputs);
}

Map::Map(Model model, InputNode inputNode, OutputNode outputNode)
{
    auto output = GetDefaultOutput(outputNode);
    const ell::model::InputNodeBase* innerInputNode = inputNode.GetInputNode();
    std::string name = innerInputNode->GetFriendlyName();
    if (name.empty())
    {
        name = "input";
    }

    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> inputs = { std::pair<std::string, ell::model::InputNodeBase*>{ name, const_cast<ell::model::InputNodeBase*>(innerInputNode) } };

    const ell::model::PortElementsBase& innerPortElements = output.GetPortElements();
    name = GetVariableName(innerPortElements, "output");
    std::shared_ptr<ell::model::Model> ellModel = model.GetModel();
    auto outputs = std::vector<std::pair<std::string, const ell::model::OutputPortBase&>>{ { name, ellModel->SimplifyOutputs(innerPortElements) } };
    _map = std::make_shared<ell::model::Map>(*ellModel, inputs, outputs);
}

Map::Map(Model model, const std::vector<InputNode*> inputNodes, const std::vector<PortElements*> outputs)
{
    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> mapInputs;

    ell::utilities::UniqueNameList inputScope;
    for (auto& inputNode : inputNodes)
    {
        const ell::model::InputNodeBase* innerInputNode = inputNode->GetInputNode();
        auto name = innerInputNode->GetFriendlyName();
        if (name.empty())
        {
            name = inputScope.Add("input");
        }
        mapInputs.push_back(std::pair<std::string, ell::model::InputNodeBase*>{ name, const_cast<ell::model::InputNodeBase*>(innerInputNode) });
    }

    auto ellModel = model.GetModel();
    std::vector<std::pair<std::string, const ell::model::OutputPortBase&>> mapOutputs;
    ell::utilities::UniqueNameList outputScope;
    for (auto& output : outputs)
    {
        const ell::model::PortElementsBase& innerPortElements = output->GetPortElements();
        auto name = outputScope.Add(GetVariableName(innerPortElements, "output"));
        mapOutputs.push_back({ name, ellModel->SimplifyOutputs(innerPortElements) });
    }
    _map = std::make_shared<ell::model::Map>(*ellModel, mapInputs, mapOutputs);
}

Map::Map(Model model, const std::vector<InputNode*> inputNodes, const std::vector<OutputNode*> outputNodes)
{
    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> mapInputs;

    ell::utilities::UniqueNameList inputScope;
    for (auto& inputNode : inputNodes)
    {
        const ell::model::InputNodeBase* innerInputNode = inputNode->GetInputNode();
        auto name = innerInputNode->GetFriendlyName();
        if (name.empty())
        {
            name = inputScope.Add("input");
        }
        mapInputs.push_back(std::pair<std::string, ell::model::InputNodeBase*>{ name, const_cast<ell::model::InputNodeBase*>(innerInputNode) });
    }

    auto ellModel = model.GetModel();
    std::vector<std::pair<std::string, const ell::model::OutputPortBase&>> mapOutputs;
    ell::utilities::UniqueNameList outputScope;
    for (auto& outputNode : outputNodes)
    {
        auto output = GetDefaultOutput(*outputNode);
        const ell::model::PortElementsBase& innerPortElements = output.GetPortElements();
        auto name = outputScope.Add(GetVariableName(innerPortElements, "output"));
        mapOutputs.push_back({ name, ellModel->SimplifyOutputs(innerPortElements) });
    }
    _map = std::make_shared<ell::model::Map>(*ellModel, mapInputs, mapOutputs);
}

Map::Map(std::shared_ptr<ell::model::Map>& map) :
    _map(map)
{
}

Map::Map(const std::string& filename)
{
    Load(filename);
}

int Map::NumInputs() const
{
    return _map->NumInputs();
}

ell::api::math::TensorShape Map::GetInputShape(int index) const
{
    return ell::api::math::TensorShape::FromMemoryShape(_map->GetInputShape(index));
}

ell::api::math::TensorShape Map::GetOutputShape(int index) const
{
    return ell::api::math::TensorShape::FromMemoryShape(_map->GetOutputShape(index));
}

PortMemoryLayout Map::GetInputLayout(int index) const
{
    return PortMemoryLayout(_map->GetInput(index)->GetMemoryLayout());
}

PortMemoryLayout Map::GetOutputLayout(int index) const
{
    return PortMemoryLayout(_map->GetOutput(index).GetMemoryLayout());
}

PortType Map::GetInputType(int index) const
{
    return (PortType)_map->GetInputType(index);
}

int Map::NumOutputs() const
{
    return _map->NumOutputs();
}

PortType Map::GetOutputType(int index) const
{
    return (PortType)_map->GetOutputType(index);
}

Model Map::GetModel() const
{
    return Model(_map->GetModel().ShallowCopy());
}

void Map::Refine(int iterations)
{
    _map->Refine(iterations);
}

void Map::Load(const std::string& filename)
{
    ell::common::MapLoadArguments args;
    args.inputMapFilename = filename;
    _map = std::make_shared<ell::model::Map>(ell::common::LoadMap(args));
    _sourceNodeState = TriState::Uninitialized;
}

void Map::Save(const std::string& filename) const
{
    ell::common::SaveMap(*_map, filename);
}

void Map::Reset()
{
    _map->Reset();
}

bool Map::HasSourceNodes()
{
    if (_sourceNodeState == TriState::Uninitialized)
    {
        // lazily search for SourceNode and cache the answer so next call is much faster.
        auto sourceNodes = _map->GetModel().GetNodesByType<ell::model::SourceNodeBase>();
        _sourceNodeState = sourceNodes.empty() ? TriState::No : TriState::Yes;
    }
    return _sourceNodeState == TriState::Yes;
}

std::string Map::GetMetadataValue(const std::string& key)
{
    std::string value;
    if (_map->GetMetadata().HasEntry(key))
    {
        value = _map->GetMetadata().GetEntry<std::string>(key);
    }
    return value;
}

std::vector<CallbackInfo> Map::GetSinkCallbackInfo()
{
    std::vector<CallbackInfo> result;
    auto floatNodes = GetModel().GetModel()->GetNodesByType<ell::nodes::SinkNode<float>>();
    for (auto node : floatNodes)
    {
        result.push_back({ node->GetCallbackName(), PortType::smallReal });
    }

    auto doubleNodes = GetModel().GetModel()->GetNodesByType<ell::nodes::SinkNode<double>>();
    for (auto node : doubleNodes)
    {
        result.push_back({ node->GetCallbackName(), PortType::real });
    }

    auto intNodes = GetModel().GetModel()->GetNodesByType<ell::nodes::SinkNode<int>>();
    for (auto node : intNodes)
    {
        result.push_back({ node->GetCallbackName(), PortType::integer });
    }
    return result;
}

std::vector<CallbackInfo> Map::GetSourceCallbackInfo()
{
    std::vector<CallbackInfo> result;
    auto floatNodes = GetModel().GetModel()->GetNodesByType<ell::nodes::SourceNode<float>>();
    for (auto node : floatNodes)
    {
        result.push_back({ node->GetCallbackName(), PortType::smallReal });
    }

    auto doubleNodes = GetModel().GetModel()->GetNodesByType<ell::nodes::SourceNode<double>>();
    for (auto node : doubleNodes)
    {
        result.push_back({ node->GetCallbackName(), PortType::real });
    }

    auto intNodes = GetModel().GetModel()->GetNodesByType<ell::nodes::SourceNode<int>>();
    for (auto node : intNodes)
    {
        result.push_back({ node->GetCallbackName(), PortType::integer });
    }
    return result;
}

std::vector<CallbackInfo> Map::GetLagCallbackInfo()
{
    std::vector<CallbackInfo> result;
    auto nodes = GetModel().GetModel()->GetNodesByType<ell::nodes::ClockNode>();
    for (auto node : nodes)
    {
        result.push_back({ node->GetLagNotificationFunctionName(), PortType::real });
    }
    return result;
}

void Map::SetMetadataValue(const std::string& key, const std::string& value)
{
    _map->GetMetadata()[key] = value;
}

std::vector<double> Map::ComputeDouble(const AutoDataVector& inputData)
{
    const ell::data::AutoDataVector& data = *(inputData._impl->_vector);
    ell::data::DenseDataVector<double> output = _map->Compute<ell::data::DenseDataVector<double>>(data);
    return output.ToArray();
}

std::vector<double> Map::ComputeDouble(const std::vector<double>& inputData)
{
    return _map->Compute<double>(inputData);
}

std::vector<float> Map::ComputeFloat(const std::vector<float>& inputData)
{
    return _map->Compute<float>(inputData);
}

CompiledMap Map::Compile(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName, const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings) const
{
    ell::model::MapCompilerOptions settings;
    settings.moduleName = moduleName;
    settings.mapFunctionName = functionName;
    settings.compilerSettings.targetDevice.deviceName = targetDevice;
    settings.compilerSettings.optimize = compilerSettings.optimize;
    settings.compilerSettings.profile = compilerSettings.profile;
    settings.compilerSettings.parallelize = compilerSettings.parallelize;
    settings.compilerSettings.useThreadPool = compilerSettings.useThreadPool;
    settings.compilerSettings.maxThreads = compilerSettings.maxThreads;
    settings.compilerSettings.useFastMath = compilerSettings.useFastMath;
    settings.compilerSettings.includeDiagnosticInfo = compilerSettings.includeDiagnosticInfo;
    settings.compilerSettings.useBlas = compilerSettings.useBlas;
    settings.compilerSettings.unrollLoops = compilerSettings.unrollLoops;
    settings.compilerSettings.inlineOperators = compilerSettings.inlineOperators;
    settings.compilerSettings.allowVectorInstructions = compilerSettings.allowVectorInstructions;
    settings.compilerSettings.vectorWidth = compilerSettings.vectorWidth;
    settings.compilerSettings.debug = compilerSettings.debug;

    ell::model::ModelOptimizerOptions optimizerOptions;
    optimizerOptions["fuseLinearFunctionNodes"] = optimizerSettings.fuseLinearFunctionNodes;

    auto compiler = std::make_shared<ell::model::IRMapCompiler>(settings, optimizerOptions);
    auto compiledMap = std::make_shared<ell::model::IRCompiledMap>(compiler->Compile(*_map));

    return CompiledMap(std::move(compiler), std::move(compiledMap), _map);
}

//
// CompiledMap
//
CompiledMap::CompiledMap(
    std::shared_ptr<ell::model::IRMapCompiler> compiler,
    std::shared_ptr<ell::model::IRCompiledMap> compiledMap,
    std::shared_ptr<ell::model::Map> map) :
    _compiler(std::move(compiler)),
    _compiledMap(std::move(compiledMap)),
    _map(map)
{}

CompiledMap::~CompiledMap()
{
}

std::string CompiledMap::GetCodeString()
{
    std::stringstream s;
    if (_compiledMap != nullptr)
    {
        _compiledMap->WriteCode(s, ell::emitters::ModuleOutputFormat::ir);
    }
    return s.str();
}

bool CompiledMap::HasSourceNodes()
{
    if (_sourceNodeState == TriState::Uninitialized)
    {
        // lazily search for SourceNode and cache the answer so next call is much faster.
        auto sourceNodes = _map->GetModel().GetNodesByType<ell::model::SourceNodeBase>();
        _sourceNodeState = sourceNodes.empty() ? TriState::No : TriState::Yes;
    }
    return _sourceNodeState == TriState::Yes;
}

int CompiledMap::NumInputs() const
{
    if (_map != nullptr)
    {
        return _map->NumInputs();
    }
    return 0;
}

ell::api::math::TensorShape CompiledMap::GetInputShape(int index) const
{
    if (_map != nullptr)
    {
        return ell::api::math::TensorShape::FromMemoryShape(_map->GetInputShape(index));
    }
    return {};
}

PortMemoryLayout CompiledMap::GetInputLayout(int index) const
{
    if (_map != nullptr)
    {
        return PortMemoryLayout(_map->GetInput(index)->GetMemoryLayout());
    }
    auto empty = ell::model::PortMemoryLayout();
    return PortMemoryLayout(empty);
}

PortType CompiledMap::GetInputType(int index) const
{
    return (PortType)_map->GetInputType(index);
}

int CompiledMap::NumOutputs() const
{
    if (_map != nullptr)
    {
        return _map->NumOutputs();
    }
    return 0;
}

ell::api::math::TensorShape CompiledMap::GetOutputShape(int index) const
{
    if (_map != nullptr)
    {
        return ell::api::math::TensorShape::FromMemoryShape(_map->GetInputShape(index));
    }
    return {};
}

PortMemoryLayout CompiledMap::GetOutputLayout(int index) const
{
    if (_map != nullptr)
    {
        return PortMemoryLayout(_map->GetOutput(index).GetMemoryLayout());
    }
    auto empty = ell::model::PortMemoryLayout();
    return PortMemoryLayout(empty);
}

PortType CompiledMap::GetOutputType(int index) const
{
    return (PortType)_map->GetOutputType(index);
}

std::vector<double> CompiledMap::ComputeDouble(const std::vector<double>& inputData)
{
    if (_compiledMap != nullptr)
    {
        return _compiledMap->Compute<double>(inputData);
    }
    return {};
}

std::vector<float> CompiledMap::ComputeFloat(const std::vector<float>& inputData)
{
    if (_compiledMap != nullptr)
    {
        return _compiledMap->Compute<float>(inputData);
    }
    return {};
}

std::vector<int> CompiledMap::ComputeInt(const std::vector<int>& inputData)
{
    if (_compiledMap != nullptr)
    {
        return _compiledMap->Compute<int>(inputData);
    }
    return {};
}
//
//std::vector<int64_t> CompiledMap::ComputeInt64(const std::vector<int64_t>& inputData)
//{
//    if (_compiledMap != nullptr)
//    {
//        return _compiledMap->Compute<int64_t>(inputData);
//    }
//    return {};
//}

void CompiledMap::Reset()
{
    if (_compiledMap != nullptr)
    {
        return _compiledMap->Reset();
    }
}

void CompiledMap::WriteIR(const std::string& filePath)
{
    if (_compiledMap != nullptr)
    {
        _compiledMap->WriteCode(filePath, ell::emitters::ModuleOutputFormat::ir);
    }
}

void CompiledMap::WriteBitcode(const std::string& filePath)
{
    if (_compiledMap != nullptr)
    {
        _compiledMap->WriteCode(filePath, ell::emitters::ModuleOutputFormat::bitcode);
    }
}

void CompiledMap::WriteSwigInterface(const std::string& filePath)
{
    if (_compiledMap != nullptr)
    {
        _compiledMap->WriteCode(filePath, ell::emitters::ModuleOutputFormat::swigInterface);
    }
}

} // namespace ELL_API
