////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelInterface.h (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelInterface.h"

// common
#include "LoadModel.h"
#include "RegisterNodeCreators.h"

// emitters
#include "ModuleEmitter.h"

// utilities
#include "JsonArchiver.h"

// model
#include "InputNode.h"
#include "OutputNode.h"
#include "DynamicMap.h"
#include "SteppableMap.h"

// nodes
#include "NeuralNetworkPredictorNode.h"
#include "SinkNode.h"
#include "SourceNode.h"

// stl
#include <chrono>

namespace ELL_API
{

//
// ELL_Port
//
ELL_PortType ELL_Port::GetOutputType()
{
    return static_cast<ELL_PortType>(_port->GetType());
}

#ifndef SWIG
ELL_Port::ELL_Port(const ell::model::Port* other)
    : _port(other)
{
}
#endif

ELL_Node ELL_Port::GetNode()
{
    return ELL_Node(_port->GetNode());
}

std::string ELL_Port::GetName()
{
    return _port->GetName();
}

std::string ELL_Port::GetRuntimeTypeName()
{
    return _port->GetRuntimeTypeName();
}

int ELL_Port::Size()
{
    return static_cast<int>(_port->Size());
}

//
// ELL_InputPortIterator
//
bool ELL_InputPortIterator::IsValid()
{
    return _i < _ports.size();
}

void ELL_InputPortIterator::Next()
{
    _i = _i + 1;
}

ELL_InputPort ELL_InputPortIterator::Get()
{
    if (!IsValid())
    {
        throw std::out_of_range("invalid iterator");
    }
    return ELL_InputPort(_ports[_i]);
}

#ifndef SWIG
ELL_InputPortIterator::ELL_InputPortIterator(std::vector<ell::model::InputPortBase*> ports)
    : _i(0), _ports(ports)
{
}
#endif

//
// ELL_OutputPortIterator
//
bool ELL_OutputPortIterator::IsValid()
{
    return _i < _ports.size();
}

void ELL_OutputPortIterator::Next()
{
    _i = _i + 1;
}

ELL_OutputPort ELL_OutputPortIterator::Get()
{
    if (!IsValid())
    {
        throw std::out_of_range("invalid iterator");
    }
    return ELL_OutputPort(_ports[_i]);
}

#ifndef SWIG
ELL_OutputPortIterator::ELL_OutputPortIterator(std::vector<ell::model::OutputPortBase*> ports)
    : _i(0), _ports(ports)
{
}
#endif

//
// ELL_NodeIterator
//
bool ELL_NodeIterator::IsValid()
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

void ELL_NodeIterator::Next()
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

ELL_Node ELL_NodeIterator::Get()
{
    if (_isVector)
    {
        if (_i >= _nodes.size())
        {
            throw std::out_of_range("invalid iterator");
        }
        return ELL_Node(_nodes[_i]);
    }
    else
    {
        return ELL_Node(_iterator.Get());
    }
}

#ifndef SWIG
ELL_NodeIterator::ELL_NodeIterator(std::vector<const ell::model::Node*> nodes)
    : _i(0), _isVector(true), _nodes(nodes), _iterator()
{
}

ELL_NodeIterator::ELL_NodeIterator(ell::model::NodeIterator& other)
    : _i(0), _isVector(false), _nodes(0), _iterator(other)
{
}
#endif

//
// ELL_Node
//
#ifndef SWIG
ELL_Node::ELL_Node(const ell::model::Node* other)
    : _node(other)
{
}
#endif

std::string ELL_Node::GetId()
{
    return to_string(_node->GetId());
}

ELL_NodeIterator ELL_Node::GetParents()
{
    return ELL_NodeIterator(_node->GetParentNodes());
}

ELL_NodeIterator ELL_Node::GetDependents()
{
    return ELL_NodeIterator(_node->GetDependentNodes());
}

ELL_OutputPort ELL_Node::GetOutputPort(const std::string& portName)
{
    auto port = _node->GetOutputPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '" + portName + "'");
    }
    return ELL_OutputPort(port);
}

ELL_InputPort ELL_Node::GetInputPort(const std::string& portName)
{
    auto port = _node->GetInputPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '" + portName + "'");
    }
    return ELL_InputPort(port);
}

ELL_Port ELL_Node::GetPort(const std::string& portName)
{
    auto port = _node->GetPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '" + portName + "'");
    }
    return ELL_Port(port);
}

ELL_OutputPortIterator ELL_Node::GetOutputPorts()
{
    return ELL_OutputPortIterator(_node->GetOutputPorts());
}

ELL_InputPortIterator ELL_Node::GetInputPorts()
{
    return ELL_InputPortIterator(_node->GetInputPorts());
}

std::string ELL_Node::GetRuntimeTypeName()
{
    return _node->GetRuntimeTypeName();
}

//
// ELL_InputNode
//
ELL_InputNode::ELL_InputNode(const ELL_InputNode& node)
    : ELL_Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::InputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an InputNode");
    }
}

ELL_InputNode::ELL_InputNode(ELL_Node node)
    : ELL_Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::InputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an InputNode");
    }
}

ELL_InputNode::ELL_InputNode(const ell::model::InputNodeBase* other)
    : ELL_Node(other)
{
}

const ell::model::InputNodeBase* ELL_InputNode::GetInputNode() const
{
    return dynamic_cast<const ell::model::InputNodeBase*>(GetNode());
}

//
// ELL_OutputNode
//
ELL_OutputNode::ELL_OutputNode(const ELL_OutputNode& node)
    : ELL_Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::OutputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an OutputNode");
    }
}

ELL_OutputNode::ELL_OutputNode(ELL_Node node)
    : ELL_Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::OutputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an OutputNode");
    }
}

ELL_OutputNode::ELL_OutputNode(const ell::model::OutputNodeBase* other)
    : ELL_Node(other)
{
}

const ell::model::OutputNodeBase* ELL_OutputNode::GetOutputNode() const
{
    return dynamic_cast<const ell::model::OutputNodeBase*>(GetNode());
}

//
// ELL_PortElement
//
#ifndef SWIG
ELL_PortElement::ELL_PortElement(const ell::model::PortElementBase& other)
    : _port(other)
{
}
#endif

int ELL_PortElement::GetIndex()
{
    return static_cast<int>(_port.GetIndex());
}

ELL_PortType ELL_PortElement::GetType()
{
    return static_cast<ELL_PortType>(_port.GetPortType());
}

ELL_OutputPort ELL_PortElement::ReferencedPort()
{
    auto port = _port.ReferencedPort();
    if (port == nullptr)
    {
        throw std::runtime_error("no referenced port");
    }
    return ELL_OutputPort(port);
}

//
// ELL_PortElements
//
ELL_PortElements::ELL_PortElements(const ell::model::PortElementsBase& other)
    : _elements(other)
{
}

ELL_PortElements::ELL_PortElements(const ELL_OutputPort& port)
    : _elements(port.GetPort())
{
}

int ELL_PortElements::Size() const
{
    return _elements.Size();
}

ELL_PortType ELL_PortElements::GetType() const
{
    return static_cast<ELL_PortType>(_elements.GetPortType());
}

ELL_PortElement ELL_PortElements::GetElement(int index) const
{
    if (index < 0 || index >= Size())
    {
        throw std::invalid_argument("index out of range");
    }
    return ELL_PortElement(_elements.GetElement(index));
}

//
// ELL_InputPort
//

#ifndef SWIG
ELL_InputPort::ELL_InputPort(const ell::model::InputPortBase* other)
    : _port(other)
{
}
#endif

ELL_PortType ELL_InputPort::GetOutputType()
{
    return static_cast<ELL_PortType>(_port->GetType());
}

ELL_Node ELL_InputPort::GetNode()
{
    return ELL_Node(_port->GetNode());
}

int ELL_InputPort::Size()
{
    return (int)_port->Size();
}

std::string ELL_InputPort::GetName()
{
    return _port->GetName();
}

std::string ELL_InputPort::GetRuntimeTypeName()
{
    return _port->GetRuntimeTypeName();
}

ELL_NodeIterator ELL_InputPort::GetParentNodes()
{
    return ELL_NodeIterator(_port->GetParentNodes());
}

ELL_PortElements ELL_InputPort::GetInputElements()
{
    return ELL_PortElements(_port->GetInputElements());
}

//
// ELL_OutputPort
//
#ifndef SWIG
ELL_OutputPort::ELL_OutputPort(const ell::model::OutputPortBase* other)
    : _port(other)
{
}
#endif

bool ELL_OutputPort::IsReferenced() const
{
    return _port->IsReferenced();
}

ELL_PortType ELL_OutputPort::GetOutputType()
{
    return static_cast<ELL_PortType>(_port->GetType());
}

std::vector<double> ELL_OutputPort::GetDoubleOutput()
{
    return _port->GetDoubleOutput();
}

double ELL_OutputPort::GetDoubleOutput(int index)
{
    return _port->GetDoubleOutput((size_t)index);
}

ELL_Node ELL_OutputPort::GetNode()
{
    return ELL_Node(_port->GetNode());
}

int ELL_OutputPort::Size()
{
    return (int)_port->Size();
}

std::string ELL_OutputPort::GetName()
{
    return _port->GetName();
}

void ELL_OutputPort::ReferencePort()
{
    _port->ReferencePort();
}

//
// ELL_Model
//

ELL_Model::ELL_Model()
{
    _model = std::make_shared<ell::model::Model>();
}
ELL_Model::ELL_Model(const std::string& filename)
{
    _model = std::make_shared<ell::model::Model>(ell::common::LoadModel(filename));
}

void ELL_Model::Save(const std::string& filename)
{
    ell::common::SaveModel(*_model, filename);
}

size_t ELL_Model::Size()
{
    return _model->Size();
}

ELL_NodeIterator ELL_Model::GetNodes()
{
    auto iter = _model->GetNodeIterator();
    return ELL_NodeIterator(iter);
}

std::string ELL_Model::GetJson() const
{
    std::stringstream stream;
    ell::utilities::JsonArchiver ar(stream);
    ar << *_model;
    return stream.str();
}

#ifndef SWIG
ELL_Model::ELL_Model(ell::model::Model&& other)
{
    _model = std::make_shared<ell::model::Model>(std::move(other));
}

ell::model::Model& ELL_Model::GetModel()
{
    return *_model;
}

#endif

//
// ELL_ModelBuilder
//
ELL_ModelBuilder::ELL_ModelBuilder()
{
    ell::common::RegisterNodeCreators(_modelBuilder);
}

ELL_Node ELL_ModelBuilder::AddNode(ELL_Model model, const std::string& nodeType, const std::vector<std::string>& args)
{
    auto newNode = _modelBuilder.AddNode(model.GetModel(), nodeType, args);
    return ELL_Node(newNode);
}

ELL_Node ELL_ModelBuilder::AddDoubleNeuralNetworkPredictorNode(ELL_Model model, ELL_PortElements input, ell::api::predictors::NeuralNetworkPredictor<double> predictor)
{
    return AddNeuralNetworkPredictorNode<double>(model, input, predictor);
}

ELL_Node ELL_ModelBuilder::AddFloatNeuralNetworkPredictorNode(ELL_Model model, ELL_PortElements input, ell::api::predictors::NeuralNetworkPredictor<float> predictor)
{
    return AddNeuralNetworkPredictorNode<float>(model, input, predictor);
}

template <typename ElementType>
ELL_Node ELL_ModelBuilder::AddNeuralNetworkPredictorNode(ELL_Model model, ELL_PortElements input, ell::api::predictors::NeuralNetworkPredictor<ElementType> predictor)
{
    auto elements = ell::model::PortElements<ElementType>(input.GetPortElements());
    auto newNode = model.GetModel().AddNode<ell::nodes::NeuralNetworkPredictorNode<ElementType>>(elements, predictor.GetPredictor());
    return ELL_Node(newNode);
}

ELL_Node ELL_ModelBuilder::AddInputNode(ELL_Model model, int size, ELL_PortType type)
{
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
        case ELL_PortType::boolean:
            newNode = model.GetModel().AddNode<ell::model::InputNode<bool>>(size);
            break;
        case ELL_PortType::integer:
            newNode = model.GetModel().AddNode<ell::model::InputNode<int>>(size);
            break;
        case ELL_PortType::real:
            newNode = model.GetModel().AddNode<ell::model::InputNode<double>>(size);
            break;
        case ELL_PortType::smallReal:
            newNode = model.GetModel().AddNode<ell::model::InputNode<float>>(size);
            break;
        default:
            throw std::invalid_argument("Error: could not create node");
    }
    return ELL_Node(newNode);
}

ELL_Node ELL_ModelBuilder::AddOutputNode(ELL_Model model, ELL_PortElements input)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
        case ELL_PortType::boolean:
            newNode = model.GetModel().AddNode<ell::model::OutputNode<bool>>(ell::model::PortElements<bool>(elements));
            break;
        case ELL_PortType::integer:
            newNode = model.GetModel().AddNode<ell::model::OutputNode<int>>(ell::model::PortElements<int>(elements));
            break;
        case ELL_PortType::real:
            newNode = model.GetModel().AddNode<ell::model::OutputNode<double>>(ell::model::PortElements<double>(elements));
            break;
        case ELL_PortType::smallReal:
            newNode = model.GetModel().AddNode<ell::model::OutputNode<float>>(ell::model::PortElements<float>(elements));
            break;
        default:
            throw std::invalid_argument("Error: could not create node");
    }
    return ELL_Node(newNode);
}

ELL_Node ELL_ModelBuilder::AddSinkNode(ELL_Model model, ELL_PortElements input, const std::string& sinkFunctionName)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
        case ELL_PortType::real:
            newNode = model.GetModel().AddNode<ell::nodes::SinkNode<double>>(
                ell::model::PortElements<double>(elements), [](const std::vector<double>&) {}, sinkFunctionName);
            break;
        case ELL_PortType::smallReal:
            newNode = model.GetModel().AddNode<ell::nodes::SinkNode<float>>(
                ell::model::PortElements<float>(elements), [](const std::vector<float>&) {}, sinkFunctionName);
            break;
        default:
            throw std::invalid_argument("Error: could not create node");
    }
    return ELL_Node(newNode);
}

// This will go away once SourceNode is refactored to remove the callback template parameter
template <typename ElementType>
bool SourceNode_EmptyCallback(std::vector<ElementType>&)
{
    return false;
}

ELL_Node ELL_ModelBuilder::AddSourceNode(ELL_Model model, ELL_PortElements input, ELL_PortType outputType, 
    int outputSize, const std::string& sourceFunctionName)
{
    auto inputType = input.GetType();
    if (inputType != ELL_PortType::real)
    {
        throw std::invalid_argument("Only ELL_PortType::real is supported for time signal input");
    }
 
    using TimeTickType = double;
    auto inputElements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (outputType)
    {
        case ELL_PortType::real:
            newNode = model.GetModel().AddNode<ell::nodes::SourceNode<double, &SourceNode_EmptyCallback<double>>>(
                ell::model::PortElements<TimeTickType>(inputElements), outputSize, sourceFunctionName);
            break;
        case ELL_PortType::smallReal:
            newNode = model.GetModel().AddNode<ell::nodes::SourceNode<float, &SourceNode_EmptyCallback<float>>>(
                ell::model::PortElements<TimeTickType>(inputElements), outputSize, sourceFunctionName);
            break;
        default:
            throw std::invalid_argument("Error: could not create node");
    }
    return ELL_Node(newNode);
}

//
// ELL_Map
//
ELL_Map::ELL_Map(ELL_Model model, ELL_InputNode inputNode, ELL_PortElements output)
{
    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> inputs = { std::pair<std::string, ell::model::InputNodeBase*>{ "input", const_cast<ell::model::InputNodeBase*>(inputNode.GetInputNode()) } };
    auto outputs = std::vector<std::pair<std::string, ell::model::PortElementsBase>>{ { "output", output.GetPortElements() } };
    _map = std::make_shared<ell::model::DynamicMap>(model.GetModel(), inputs, outputs);
}

std::vector<double> ELL_Map::ComputeDouble(const std::vector<double>& inputData)
{
    return _map->Compute<double>(inputData);
}

std::vector<float> ELL_Map::ComputeFloat(const std::vector<float>& inputData)
{
    return _map->Compute<float>(inputData);
}

void ELL_Map::Save(const std::string& filePath) const
{
    ell::common::SaveMap(*_map, filePath);
}

//
// ELL_SteppableMap
//
ELL_SteppableMap::ELL_SteppableMap(ELL_Model model, ELL_InputNode inputNode, ELL_PortElements output, ELL_ClockType clockType, int millisecondInterval) : _clockType(clockType)
{
    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> inputs = { std::pair<std::string, ell::model::InputNodeBase*>{ "input", const_cast<ell::model::InputNodeBase*>(inputNode.GetInputNode()) } };
    auto outputs = std::vector<std::pair<std::string, ell::model::PortElementsBase>>{ { "output", output.GetPortElements() } };
    auto duration = ell::model::DurationType(millisecondInterval);

    switch (clockType)
    {
        case ELL_ClockType::steadyClock:
            _map = std::make_shared<ell::model::SteppableMap<std::chrono::steady_clock>>(model.GetModel(), inputs, outputs, duration);
            break;
        case ELL_ClockType::systemClock:
            _map = std::make_shared<ell::model::SteppableMap<std::chrono::system_clock>>(model.GetModel(), inputs, outputs, duration);
            break;
        default:
            throw std::invalid_argument("Error: could not create map");
    }
}

void ELL_SteppableMap::Save(const std::string& filePath) const
{
    switch (_clockType)
    {
        case ELL_ClockType::steadyClock:
        {
            auto map = std::dynamic_pointer_cast<const ell::model::SteppableMap<std::chrono::steady_clock>>(_map);
            assert(map != nullptr); // coding error
            ell::common::SaveMap(*map, filePath);
            break;
        }
        case ELL_ClockType::systemClock:
        {
            auto map = std::dynamic_pointer_cast<const ell::model::SteppableMap<std::chrono::system_clock>>(_map);
            assert(map != nullptr); // coding error
            ell::common::SaveMap(*map, filePath);
            break;
        }
        default:
            assert(false); // coding error
    }
}

//
// ELL_CompiledMap
//
ELL_CompiledMap::ELL_CompiledMap(ell::model::IRCompiledMap map)
{
    _map = std::make_shared<ell::model::IRCompiledMap>(std::move(map));
}

std::vector<double> ELL_CompiledMap::ComputeDouble(const std::vector<double>& inputData)
{
    return _map->Compute<double>(inputData);
}

std::vector<float> ELL_CompiledMap::ComputeFloat(const std::vector<float>& inputData)
{
    return _map->Compute<float>(inputData);
}

std::string ELL_CompiledMap::GetCodeString()
{
    std::stringstream s;
    if (_map != nullptr)
    {
        _map->WriteCode(s, ell::emitters::ModuleOutputFormat::ir);
    }
    return s.str();
}

//
// Functions
//

ELL_Model ELL_LoadModel(std::string filename)
{
    return ELL_Model(filename);
}

ELL_Model ELL_LoadModelFromString(std::string str)
{
    std::stringstream stream(str);
    ell::utilities::SerializationContext context;
    ell::utilities::JsonUnarchiver ar(stream, context);
    ell::model::Model model;
    ar >> model;
    return ELL_Model(std::move(model));
}
}
