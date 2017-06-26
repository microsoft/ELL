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

namespace ELL_API
{

//
// ELL_Port
//
int ELL_Port::GetOutputType()
{
    return (int)(_port->GetType());
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
    return (int)_port->Size();
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
    if (port == nullptr) { throw std::invalid_argument("no port named '" + portName + "'"); }
    return ELL_OutputPort(port);
}

ELL_InputPort ELL_Node::GetInputPort(const std::string& portName)
{
    auto port = _node->GetInputPort(portName);
    if (port == nullptr) { throw std::invalid_argument("no port named '" + portName + "'"); }
    return ELL_InputPort(port);
}

ELL_Port ELL_Node::GetPort(const std::string& portName)
{
    auto port = _node->GetPort(portName);
    if (port == nullptr) { throw std::invalid_argument("no port named '" + portName + "'"); }
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
    return (int)(_port.GetIndex());
}

int ELL_PortElement::GetType()
{
    return (int)(_port.GetPortType());
}

ELL_OutputPort ELL_PortElement::ReferencedPort()
{
    auto port = _port.ReferencedPort();
    if (port == nullptr) { throw std::runtime_error("no referenced port"); }
    return ELL_OutputPort(port);
}

//
// ELL_PortElements
//
#ifndef SWIG
ELL_PortElements::ELL_PortElements(const ell::model::PortElementsBase& other)
    : _elements(other)
{
}
#endif

int ELL_PortElements::Size() const
{
    return _elements.Size();
}

int ELL_PortElements::GetType() const
{
    return static_cast<int>(_elements.GetPortType());
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

int ELL_InputPort::GetOutputType()
{
    return (int)(_port->GetType());
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

int ELL_OutputPort::GetOutputType()
{
    return (int)(_port->GetType());
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

//
// ELL_TransformContext
//

//
// ELL_DynamicMap
//
std::vector<double> ELL_DynamicMap::Compute(const std::vector<double>& inputData)
{
    _map->SetInputValue(0, inputData);
    return _map->ComputeOutput<double>(0);
}

//
// ELL_CompiledMap
//
std::string ELL_CompiledMap::GetCodeString()
{
    std::stringstream s;
    if (_map != nullptr)
    {
        _map->WriteCode(s, ell::emitters::ModuleOutputFormat::ir);
    }
    return s.str();
}

std::vector<double> ELL_CompiledMap::Compute(const std::vector<double>& inputData)
{
    _map->SetInputValue(0, inputData);
    return _map->ComputeOutput<double>(0);
}

//
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
