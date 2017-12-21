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
#include "MapLoadArguments.h"

// nodes
#include "ClockNode.h"
#include "NeuralNetworkPredictorNode.h"
#include "SinkNode.h"
#include "SourceNode.h"
#include "Tensor.h"

// stl
#include <chrono>

namespace ELL_API
{

//
// Port
//
PortType Port::GetOutputType()
{
    return static_cast<PortType>(_port->GetType());
}

#ifndef SWIG
Port::Port(const ell::model::Port* other)
    : _port(other)
{
}
#endif

Node Port::GetNode()
{
    return Node(_port->GetNode());
}

std::string Port::GetName()
{
    return _port->GetName();
}

std::string Port::GetRuntimeTypeName()
{
    return _port->GetRuntimeTypeName();
}

int Port::Size()
{
    return static_cast<int>(_port->Size());
}

//
// InputPortIterator
//
bool InputPortIterator::IsValid()
{
    return _i < _ports.size();
}

void InputPortIterator::Next()
{
    _i = _i + 1;
}

InputPort InputPortIterator::Get()
{
    if (!IsValid())
    {
        throw std::out_of_range("invalid iterator");
    }
    return InputPort(_ports[_i]);
}

#ifndef SWIG
InputPortIterator::InputPortIterator(std::vector<ell::model::InputPortBase*> ports)
    : _i(0), _ports(ports)
{
}
#endif

//
// OutputPortIterator
//
bool OutputPortIterator::IsValid()
{
    return _i < _ports.size();
}

void OutputPortIterator::Next()
{
    _i = _i + 1;
}

OutputPort OutputPortIterator::Get()
{
    if (!IsValid())
    {
        throw std::out_of_range("invalid iterator");
    }
    return OutputPort(_ports[_i]);
}

#ifndef SWIG
OutputPortIterator::OutputPortIterator(std::vector<ell::model::OutputPortBase*> ports)
    : _i(0), _ports(ports)
{
}
#endif

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
        return Node(_nodes[_i]);
    }
    else
    {
        return Node(_iterator.Get());
    }
}

#ifndef SWIG
NodeIterator::NodeIterator(std::vector<const ell::model::Node*> nodes)
    : _i(0), _isVector(true), _nodes(nodes), _iterator()
{
}

NodeIterator::NodeIterator(ell::model::NodeIterator& other)
    : _i(0), _isVector(false), _nodes(0), _iterator(other)
{
}
#endif

//
// Node
//
#ifndef SWIG
Node::Node(const ell::model::Node* other)
    : _node(other)
{
}
#endif

std::string Node::GetId()
{
    return to_string(_node->GetId());
}

NodeIterator Node::GetParents()
{
    return NodeIterator(_node->GetParentNodes());
}

NodeIterator Node::GetDependents()
{
    return NodeIterator(_node->GetDependentNodes());
}

OutputPort Node::GetOutputPort(const std::string& portName)
{
    auto port = _node->GetOutputPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '" + portName + "'");
    }
    return OutputPort(port);
}

InputPort Node::GetInputPort(const std::string& portName)
{
    auto port = _node->GetInputPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '" + portName + "'");
    }
    return InputPort(port);
}

Port Node::GetPort(const std::string& portName)
{
    auto port = _node->GetPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '" + portName + "'");
    }
    return Port(port);
}

OutputPortIterator Node::GetOutputPorts()
{
    return OutputPortIterator(_node->GetOutputPorts());
}

InputPortIterator Node::GetInputPorts()
{
    return InputPortIterator(_node->GetInputPorts());
}

std::string Node::GetRuntimeTypeName()
{
    return _node->GetRuntimeTypeName();
}

//
// InputNode
//
InputNode::InputNode(const InputNode& node)
    : Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::InputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an InputNode");
    }
}

InputNode::InputNode(Node node)
    : Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::InputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an InputNode");
    }
}

InputNode::InputNode(const ell::model::InputNodeBase* other)
    : Node(other)
{
}

const ell::model::InputNodeBase* InputNode::GetInputNode() const
{
    return dynamic_cast<const ell::model::InputNodeBase*>(GetNode());
}

//
// OutputNode
//
OutputNode::OutputNode(const OutputNode& node)
    : Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::OutputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an OutputNode");
    }
}

OutputNode::OutputNode(Node node)
    : Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::OutputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an OutputNode");
    }
}

OutputNode::OutputNode(const ell::model::OutputNodeBase* other)
    : Node(other)
{
}

const ell::model::OutputNodeBase* OutputNode::GetOutputNode() const
{
    return dynamic_cast<const ell::model::OutputNodeBase*>(GetNode());
}

//
// PortElement
//
#ifndef SWIG
PortElement::PortElement(const ell::model::PortElementBase& other)
    : _port(other)
{
}
#endif

int PortElement::GetIndex()
{
    return static_cast<int>(_port.GetIndex());
}

PortType PortElement::GetType()
{
    return static_cast<PortType>(_port.GetPortType());
}

OutputPort PortElement::ReferencedPort()
{
    auto port = _port.ReferencedPort();
    if (port == nullptr)
    {
        throw ell::utilities::Exception("no referenced port");
    }
    return OutputPort(port);
}

//
// PortElements
//
PortElements::PortElements(const ell::model::PortElementsBase& other)
    : _elements(other)
{
}

PortElements::PortElements(const OutputPort& port)
    : _elements(port.GetPort())
{
}

int PortElements::Size() const
{
    return _elements.Size();
}

PortType PortElements::GetType() const
{
    return static_cast<PortType>(_elements.GetPortType());
}

PortElement PortElements::GetElement(int index) const
{
    if (index < 0 || index >= Size())
    {
        throw std::invalid_argument("index out of range");
    }
    return PortElement(_elements.GetElement(index));
}

//
// InputPort
//

#ifndef SWIG
InputPort::InputPort(const ell::model::InputPortBase* other)
    : _port(other)
{
}
#endif

PortType InputPort::GetOutputType()
{
    return static_cast<PortType>(_port->GetType());
}

Node InputPort::GetNode()
{
    return Node(_port->GetNode());
}

int InputPort::Size()
{
    return (int)_port->Size();
}

std::string InputPort::GetName()
{
    return _port->GetName();
}

std::string InputPort::GetRuntimeTypeName()
{
    return _port->GetRuntimeTypeName();
}

NodeIterator InputPort::GetParentNodes()
{
    return NodeIterator(_port->GetParentNodes());
}

PortElements InputPort::GetInputElements()
{
    return PortElements(_port->GetInputElements());
}

//
// OutputPort
//
#ifndef SWIG
OutputPort::OutputPort(const ell::model::OutputPortBase* other)
    : _port(other)
{
}
#endif

bool OutputPort::IsReferenced() const
{
    return _port->IsReferenced();
}

PortType OutputPort::GetOutputType()
{
    return static_cast<PortType>(_port->GetType());
}

std::vector<double> OutputPort::GetDoubleOutput()
{
    return _port->GetDoubleOutput();
}

double OutputPort::GetDoubleOutput(int index)
{
    return _port->GetDoubleOutput((size_t)index);
}

Node OutputPort::GetNode()
{
    return Node(_port->GetNode());
}

int OutputPort::Size()
{
    return (int)_port->Size();
}

std::string OutputPort::GetName()
{
    return _port->GetName();
}

void OutputPort::ReferencePort()
{
    _port->ReferencePort();
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
    _model = std::make_shared<ell::model::Model>(ell::common::LoadModel(filename));
}

void Model::Save(const std::string& filename)
{
    ell::common::SaveModel(*_model, filename);
}

size_t Model::Size()
{
    return _model->Size();
}

NodeIterator Model::GetNodes()
{
    auto iter = _model->GetNodeIterator();
    return NodeIterator(iter);
}

std::string Model::GetJson() const
{
    std::stringstream stream;
    ell::utilities::JsonArchiver ar(stream);
    ar << *_model;
    return stream.str();
}

Model Model::Refine(int maxIterations) 
{
    ell::model::TransformContext context;
    ell::model::ModelTransformer transformer;
    ell::model::Model refinedModel = transformer.RefineModel(*_model, context, maxIterations);
    return Model(std::move(refinedModel));
}


#ifndef SWIG
Model::Model(ell::model::Model&& other)
{
    _model = std::make_shared<ell::model::Model>(std::move(other));
}

ell::model::Model& Model::GetModel()
{
    return *_model;
}

#endif

//
// ModelBuilder
//
ModelBuilder::ModelBuilder()
{
    ell::common::RegisterNodeCreators(_modelBuilder);
}

Node ModelBuilder::AddNode(Model model, const std::string& nodeType, const std::vector<std::string>& args)
{
    auto newNode = _modelBuilder.AddNode(model.GetModel(), nodeType, args);
    return Node(newNode);
}

Node ModelBuilder::AddDoubleNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<double> predictor)
{
    return AddNeuralNetworkPredictorNode<double>(model, input, predictor);
}

Node ModelBuilder::AddFloatNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<float> predictor)
{
    return AddNeuralNetworkPredictorNode<float>(model, input, predictor);
}

template <typename ElementType>
Node ModelBuilder::AddNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<ElementType> predictor)
{
    auto elements = ell::model::PortElements<ElementType>(input.GetPortElements());
    auto newNode = model.GetModel().AddNode<ell::nodes::NeuralNetworkPredictorNode<ElementType>>(elements, predictor.GetPredictor());
    return Node(newNode);
}

Node ModelBuilder::AddInputNode(Model model, const ell::api::math::TensorShape& tensorShape, PortType type)
{
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
        case PortType::boolean:
            newNode = model.GetModel().AddNode<ell::model::InputNode<bool>>(tensorShape.ToMathTensorShape());
            break;
        case PortType::integer:
            newNode = model.GetModel().AddNode<ell::model::InputNode<int>>(tensorShape.ToMathTensorShape());
            break;
        case PortType::real:
            newNode = model.GetModel().AddNode<ell::model::InputNode<double>>(tensorShape.ToMathTensorShape());
            break;
        case PortType::smallReal:
            newNode = model.GetModel().AddNode<ell::model::InputNode<float>>(tensorShape.ToMathTensorShape());
            break;
        default:
            throw std::invalid_argument("Error: could not create node");
    }
    return Node(newNode);
}

Node ModelBuilder::AddOutputNode(Model model, const ell::api::math::TensorShape& tensorShape, PortElements input)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
        case PortType::boolean:
            newNode = model.GetModel().AddNode<ell::model::OutputNode<bool>>(ell::model::PortElements<bool>(elements), tensorShape.ToMathTensorShape());
            break;
        case PortType::integer:
            newNode = model.GetModel().AddNode<ell::model::OutputNode<int>>(ell::model::PortElements<int>(elements), tensorShape.ToMathTensorShape());
            break;
        case PortType::real:
            newNode = model.GetModel().AddNode<ell::model::OutputNode<double>>(ell::model::PortElements<double>(elements), tensorShape.ToMathTensorShape());
            break;
        case PortType::smallReal:
            newNode = model.GetModel().AddNode<ell::model::OutputNode<float>>(ell::model::PortElements<float>(elements), tensorShape.ToMathTensorShape());
            break;
        default:
            throw std::invalid_argument("Error: could not create node");
    }
    return Node(newNode);
}

Node ModelBuilder::AddClockNode(Model model, PortElements input, double interval, short lagThreshold, const std::string& lagNotificationName)
{
    auto elements = input.GetPortElements();
    auto newNode = model.GetModel().AddNode<ell::nodes::ClockNode>(
        ell::model::PortElements<ell::nodes::TimeTickType>(elements),
        ell::nodes::TimeTickType(interval),
        lagThreshold,
        lagNotificationName);
    return Node(newNode);
}

Node ModelBuilder::AddSinkNode(Model model, PortElements input, const ell::api::math::TensorShape& tensorShape, const std::string& sinkFunctionName)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
        case PortType::real:
            newNode = model.GetModel().AddNode<ell::nodes::SinkNode<double>>(
                ell::model::PortElements<double>(elements), tensorShape.ToMathTensorShape(), sinkFunctionName);
            break;
        case PortType::smallReal:
            newNode = model.GetModel().AddNode<ell::nodes::SinkNode<float>>(
                ell::model::PortElements<float>(elements), tensorShape.ToMathTensorShape(), sinkFunctionName);
            break;
        default:
            throw std::invalid_argument("Error: could not create node");
    }
    return Node(newNode);
}

Node ModelBuilder::AddSourceNode(Model model, PortElements input, PortType outputType, 
    const ell::api::math::TensorShape& tensorShape, const std::string& sourceFunctionName)
{
    auto inputType = input.GetType();
    if (inputType != PortType::real)
    {
        throw std::invalid_argument("Only PortType::real is supported for time signal input");
    }
 
    using TimeTickType = double;
    auto inputElements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (outputType)
    {
        case PortType::real:
            newNode = model.GetModel().AddNode<ell::nodes::SourceNode<double>>(
                ell::model::PortElements<TimeTickType>(inputElements), tensorShape.ToMathTensorShape(), sourceFunctionName);
            break;
        case PortType::smallReal:
            newNode = model.GetModel().AddNode<ell::nodes::SourceNode<float>>(
                ell::model::PortElements<TimeTickType>(inputElements), tensorShape.ToMathTensorShape(), sourceFunctionName);
            break;
        default:
            throw std::invalid_argument("Error: could not create node");
    }
    return Node(newNode);
}

//
// Map
//
Map::Map()
{
    _map = std::make_shared<ell::model::DynamicMap>();
}

Map::Map(Model model, InputNode inputNode, PortElements output)
{
    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> inputs = { std::pair<std::string, ell::model::InputNodeBase*>{ "input", const_cast<ell::model::InputNodeBase*>(inputNode.GetInputNode()) } };
    auto outputs = std::vector<std::pair<std::string, ell::model::PortElementsBase>>{ { "output", output.GetPortElements() } };
    _map = std::make_shared<ell::model::DynamicMap>(model.GetModel(), inputs, outputs);
}

Map::Map(std::shared_ptr<ell::model::DynamicMap>& map)
    : _map(map)
{
}

Map::Map(const std::string& filename)
{
    Load(filename);
}

ell::api::math::TensorShape Map::GetInputShape() const
{
    return ell::api::math::TensorShape::FromMathTensorShape(_map->GetInputShape());
}

ell::api::math::TensorShape Map::GetOutputShape() const
{
    return ell::api::math::TensorShape::FromMathTensorShape(_map->GetOutputShape());
}


std::vector<double> Map::ComputeDouble(const AutoDataVector& inputData)
{
    const ell::data::AutoDataVector& data = *(inputData._impl->_vector);
    // not sure why the template can't match these types...
    //return _map->Compute<double>(data);
    _map->SetInputValue(0, data);
    return _map->ComputeOutput<double>(0);
}

Model Map::GetModel() const
{
    ell::model::Model model = _map->GetModel();
    return Model(std::move(model));
}

std::vector<double> Map::ComputeDouble(const std::vector<double>& inputData)
{
    return _map->Compute<double>(inputData);
}

std::vector<float> Map::ComputeFloat(const std::vector<float>& inputData)
{
    return _map->Compute<float>(inputData);
}

void Map::Load(const std::string& filename)
{
    ell::common::MapLoadArguments args;
    args.inputMapFilename = filename;
    _map = std::make_shared<ell::model::DynamicMap>(ell::common::LoadMap(args));
}

void Map::Save(const std::string& filename) const
{
    ell::common::SaveMap(*_map, filename);
}

CompiledMap Map::Compile(const std::string&  targetDevice, const std::string& moduleName, const std::string& functionName, bool useBlas) const
{
    ell::model::MapCompilerParameters settings;
    settings.moduleName = moduleName;
    settings.mapFunctionName = functionName;
    settings.compilerSettings.targetDevice.deviceName = targetDevice;
    settings.compilerSettings.useBlas = useBlas;

    ell::model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(*_map);
    return CompiledMap(std::move(compiledMap));
}

//
// CompiledMap
//
CompiledMap::CompiledMap(ell::model::IRCompiledMap map) 
{
    _map = std::make_shared<ell::model::IRCompiledMap>(std::move(map));
}

std::vector<double> CompiledMap::ComputeDouble(const std::vector<double>& inputData)
{
    return _map->Compute<double>(inputData);
}

std::vector<float> CompiledMap::ComputeFloat(const std::vector<float>& inputData)
{
    return _map->Compute<float>(inputData);
}

std::string CompiledMap::GetCodeString()
{
    std::stringstream s;
    if (_map != nullptr)
    {
        _map->WriteCode(s, ell::emitters::ModuleOutputFormat::ir);
    }
    return s.str();
}

void CompiledMap::WriteIR(const std::string& filePath)
{
    if (_map != nullptr)
    {
        _map->WriteCode(filePath, ell::emitters::ModuleOutputFormat::ir);
    }
}

void CompiledMap::WriteBitcode(const std::string& filePath)
{
    if (_map != nullptr)
    {
        _map->WriteCode(filePath, ell::emitters::ModuleOutputFormat::bitcode);
    }
}

void CompiledMap::WriteSwigInterface(const std::string& filePath)
{
    if (_map != nullptr)
    {
        _map->WriteCode(filePath, ell::emitters::ModuleOutputFormat::swigInterface);
    }
}



//
// Helper Functions
//


Model LoadModelFromString(std::string str)
{
    std::stringstream stream(str);
    ell::utilities::SerializationContext context;
    ell::utilities::JsonUnarchiver ar(stream, context);
    ell::model::Model model;
    ar >> model;
    return Model(std::move(model));
}
}
