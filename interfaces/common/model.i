////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     model.i (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT

// common
#include "LoadModel.h"

// compiler
#include "CompiledMap.h"

// model
#include "InputNode.h"
#include "InputPort.h"
#include "Model.h"
#include "Node.h"
#include "OutputNode.h"
#include "OutputPort.h"
#include "Port.h"
#include "PortElements.h"

// nodes
#include "ConstantNode.h"
#include "DTWDistanceNode.h"
#include "ExtremalValueNode.h"
#include "MultiplexerNode.h"

// utilities
#include "JsonArchiver.h"

// stl
#include <memory>
#include <string>
#include <sstream>
#include <vector>
%}

#if 0
%nodefaultctor ell::model::NodeIterator;
%nodefaultctor ell::model::Node;
%nodefaultctor ell::model::Port;
%nodefaultctor ell::model::OutputPortBase;
%nodefaultctor ell::model::OutputPort<double>;
%nodefaultctor ell::model::InputPortBase;
#endif

%inline %{

namespace ELL_API
{
//
// Forward declartions
//

class ELL_InputPortBaseIterator;
class ELL_OutputPortBaseIterator;
class ELL_NodeIterator;
class ELL_Port;
class ELL_InputPortBase;
class ELL_OutputPortBase;

//
// Class Declarations
//

//
// ELL_Node Class Declaration
//

class ELL_Node 
{
public:
    ELL_Node();
    ~ELL_Node();
    ELL_Node(const ELL_Node&) = default;
    std::string GetId();
    ELL_NodeIterator GetParents();
    ELL_NodeIterator GetDependents();
    ELL_OutputPortBase GetOutputPort(std::string& portName);
    ELL_InputPortBase GetInputPort(std::string portName);
    ELL_Port GetPort(const std::string& portName);
    ELL_OutputPortBaseIterator GetOutputPorts();
    ELL_InputPortBaseIterator GetInputPorts();
    std::string GetRuntimeTypeName();
#ifndef SWIG
    ELL_Node(const ell::model::Node* other);
#endif
private:
    const ell::model::Node* _node;
};

//
// ELL_InputPortBaseIterator Class Declaration
//

class ELL_InputPortBaseIterator 
{
public:
    ELL_InputPortBaseIterator();
    ~ELL_InputPortBaseIterator();
    bool IsValid();
    void Next();
    ELL_InputPortBase Get();
#ifndef SWIG
    ELL_InputPortBaseIterator(std::vector<ell::model::InputPortBase*> ports);
#endif
private:
    int _i;
    std::vector<ell::model::InputPortBase*> _ports;
};

//
// ELL_OutputPortBaseIterator Class Declaration
//

class ELL_OutputPortBaseIterator 
{
public:
    ELL_OutputPortBaseIterator();
    ~ELL_OutputPortBaseIterator();
    bool IsValid();
    void Next();
    ELL_OutputPortBase Get();
#ifndef SWIG
    ELL_OutputPortBaseIterator(std::vector<ell::model::OutputPortBase*> ports);
#endif
private:
    int _i;
    std::vector<ell::model::OutputPortBase*> _ports;
};

//
// ELL_NodeIterator Class Declaration
//

class ELL_NodeIterator 
{
public:
    ELL_NodeIterator();
    ~ELL_NodeIterator();
    bool IsValid();
    void Next();
    ELL_Node Get();
#ifndef SWIG
    ELL_NodeIterator(std::vector<const ell::model::Node*> nodes);
    ELL_NodeIterator(ell::model::NodeIterator& other);
#endif    
private:
    int _i;
    bool _isVector;
    std::vector<const ell::model::Node*> _nodes;
    ell::model::NodeIterator _iterator;
};

//
// ELL_Model Class Declaration
//

class ELL_Model 
{
public:
    ELL_Model();
    ELL_Model(const std::string& filename);
    void Save(const std::string& filename);
    size_t Size(); 
    ELL_NodeIterator GetNodes();
    std::string GetJson() const;
    std::string GetRuntimeTypeName() const;
#ifndef SWIG
    ELL_Model(const ell::model::Model& other);
#endif
private:
    ell::model::Model _model;
};

//
// ELL_Port Class Declaration
//

class ELL_Port 
{
public:
    enum class Type
    {
        none,
        real,
        integer,
        categorical,
        boolean
    };
    ELL_Port();
    ~ELL_Port();
    ELL_Node GetNode();
    std::string GetName();
    std::string GetRuntimeTypeName();
    int GetOutputType();
    int Size();
#ifndef SWIG
    ELL_Port(const ell::model::Port* other);
#endif
private:
    const ell::model::Port* _port;
};

//
// ELL_PortElementBase Class Declaration
//

class ELL_PortElementBase 
{
public:
    ELL_PortElementBase();
    ~ELL_PortElementBase();
    int GetIndex();
    int GetType();
    ELL_OutputPortBase ReferencedPort();

#ifndef SWIG
    ELL_PortElementBase(const ell::model::PortElementBase& other);
#endif
private:
    ell::model::PortElementBase _port;
};

//
// ELL_PortElementsBase Class Declaration
//

class ELL_PortElementsBase 
{
public:
    ELL_PortElementsBase() = default;
    ~ELL_PortElementsBase() = default;
    int Size() const;
    int GetType() const;
    ELL_PortElementBase GetElement(int index) const;

#ifndef SWIG
    ELL_PortElementsBase(const ell::model::PortElementsBase& other);
#endif
private:
    ell::model::PortElementsBase _elements;
};

//
// ELL_InputPortBase Class Declaration
//

class ELL_InputPortBase 
{
public:
    ELL_InputPortBase();
    ~ELL_InputPortBase();
    int Size();
    ELL_Node GetNode();
    std::string GetName();
    int GetOutputType();
    std::string GetRuntimeTypeName();
    ELL_NodeIterator GetParentNodes();
    ELL_PortElementsBase GetInputElements();
#ifndef SWIG
    ELL_InputPortBase(const ell::model::InputPortBase* other);
#endif
private:
    const ell::model::InputPortBase* _port;
};

//
// ELL_OutputPortBase Class Declaration
//

class ELL_OutputPortBase 
{
public:
    ELL_OutputPortBase();
    ~ELL_OutputPortBase();
    bool IsReferenced() const;
    std::vector<double> GetDoubleOutput();
    double GetDoubleOutput(int index);
    int Size();
    ELL_Node GetNode();
    std::string GetName();
    int GetOutputType();
    void ReferencePort();
    std::string GetRuntimeTypeName();
#ifndef SWIG
    ELL_OutputPortBase(const ell::model::OutputPortBase* other);
#endif
private:
    const ell::model::OutputPortBase* _port;
};

class ELL_TransformContext
{
public:
    ELL_TransformContext();
    ~ELL_TransformContext();
#ifndef SWIG
//    ELL_TransformContext(const std::function<bool(const Node&)>& isNodeCompilable);
#endif
private:
    ell::model::TransformContext _context;
};

ELL_TransformContext::ELL_TransformContext() : _context()
{
}

ELL_TransformContext::~ELL_TransformContext()
{
}


#ifndef SWIG
//ELL_TransformContext(const std::function<bool(const Node&)>& isNodeCompilable) :
//    _context(isNodeCompilable)
//{
//}
#endif

//
// Class Methods
//

//
// ELL_PortElementBase Methods
//

#ifndef SWIG
ELL_PortElementBase::ELL_PortElementBase(const ell::model::PortElementBase& other) : 
    _port(other) 
{
} 
#endif

ELL_PortElementBase::ELL_PortElementBase() : _port() 
{
}

ELL_PortElementBase::~ELL_PortElementBase() 
{
}

int ELL_PortElementBase::GetIndex() 
{
    return (int)(_port.GetIndex());
}

int ELL_PortElementBase::GetType() 
{
    return (int)(_port.GetType());
}

ELL_OutputPortBase ELL_PortElementBase::ReferencedPort() 
{
    return ELL_OutputPortBase(_port.ReferencedPort());
}

//
// ELL_PortElementsBase Methods
//

#ifndef SWIG
ELL_PortElementsBase::ELL_PortElementsBase(const ell::model::PortElementsBase& other) : 
    _elements(other) 
{
} 
#endif

int ELL_PortElementsBase::Size() const
{
    return _elements.Size();
}

int ELL_PortElementsBase::GetType() const
{
    return static_cast<int>(_elements.GetType());
}

ELL_PortElementBase ELL_PortElementsBase::GetElement(int index) const
{
    return ELL_PortElementBase(_elements.GetElement(index));
}

//
// ELL_OutputPortBase Methods 
//

#ifndef SWIG
ELL_OutputPortBase::ELL_OutputPortBase(const ell::model::OutputPortBase* other) : 
    _port(other) 
{
}
#endif

ELL_OutputPortBase::ELL_OutputPortBase() : _port(nullptr) 
{
}

ELL_OutputPortBase::~ELL_OutputPortBase() 
{
}

bool ELL_OutputPortBase::IsReferenced() const
{
    return _port->IsReferenced();
}

int ELL_OutputPortBase::GetOutputType()
{
    return (int) (_port->GetType());
}

std::vector<double> ELL_OutputPortBase::GetDoubleOutput() 
{
    return _port->GetDoubleOutput();
}

double ELL_OutputPortBase::GetDoubleOutput(int index) 
{
    return _port->GetDoubleOutput((size_t) index);
}

ELL_Node ELL_OutputPortBase::GetNode() 
{
    return ELL_Node(_port->GetNode());
}

int ELL_OutputPortBase::Size() 
{
    return (int) _port->Size();
}

std::string ELL_OutputPortBase::GetName() 
{
    return _port->GetName();
}

void ELL_OutputPortBase::ReferencePort() 
{
    _port->ReferencePort();
}

std::string ELL_OutputPortBase::GetRuntimeTypeName() 
{
    return _port->GetRuntimeTypeName();
}

//
// ELL_InputPortBase Methods
//

#ifndef SWIG
ELL_InputPortBase::ELL_InputPortBase(const ell::model::InputPortBase* other) : 
    _port(other) {
}
#endif

ELL_InputPortBase::ELL_InputPortBase() : _port(nullptr) 
{
}

ELL_InputPortBase::~ELL_InputPortBase() 
{
}

int ELL_InputPortBase::GetOutputType()
{
    return (int)(_port->GetType());
}

ELL_Node ELL_InputPortBase::GetNode() 
{
    return ELL_Node(_port->GetNode());
}

int ELL_InputPortBase::Size() 
{
    return  (int) _port->Size();
}

std::string ELL_InputPortBase::GetName() 
{
    return _port->GetName();
}

std::string ELL_InputPortBase::GetRuntimeTypeName() 
{
    return _port->GetRuntimeTypeName();
}

ELL_NodeIterator ELL_InputPortBase::GetParentNodes() 
{
    return ELL_NodeIterator(_port->GetParentNodes());
}

ELL_PortElementsBase ELL_InputPortBase::GetInputElements()
{
    return ELL_PortElementsBase(_port->GetInputElements());
}

//
// ELL_Port Methods 
//

ELL_Port::ELL_Port() : _port(nullptr) 
{
}

ELL_Port::~ELL_Port() 
{
}

int ELL_Port::GetOutputType()
{
    return (int) (_port->GetType());
}

#ifndef SWIG
ELL_Port::ELL_Port(const ell::model::Port* other) : _port(other) 
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
    return (int) _port->Size();
}

//
// ELL_NodeIterator Methods 
//

ELL_NodeIterator::ELL_NodeIterator() : 
    _i(0), _isVector(false), _nodes(), _iterator() 
{
}

ELL_NodeIterator::~ELL_NodeIterator() 
{
}

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
        return ELL_Node(_nodes[_i]);
    }
    else
    {
        return ELL_Node(_iterator.Get());
    }
}

#ifndef SWIG
ELL_NodeIterator::ELL_NodeIterator(std::vector<const ell::model::Node*> nodes) : 
    _nodes(nodes) , _i(0), _isVector(true), _iterator() 
{
}

ELL_NodeIterator::ELL_NodeIterator(ell::model::NodeIterator& other) : 
    _nodes(0), _i(0), _isVector(false), _iterator(other) 
{
}
#endif

//
// ELL_InputPortBaseIterator Methods
//

ELL_InputPortBaseIterator::ELL_InputPortBaseIterator() : 
    _i(0), _ports() 
{
}

ELL_InputPortBaseIterator::~ELL_InputPortBaseIterator() 
{
}

bool ELL_InputPortBaseIterator::IsValid() 
{
    return _i < _ports.size();
}

void ELL_InputPortBaseIterator::Next() 
{
    _i = _i + 1;
}

ELL_InputPortBase ELL_InputPortBaseIterator::Get() 
{
    return ELL_InputPortBase(_ports[_i]);
}

#ifndef SWIG
ELL_InputPortBaseIterator::ELL_InputPortBaseIterator(std::vector<ell::model::InputPortBase*> ports) : 
    _i(0), _ports(ports) 
{
}
#endif

class ELL_CompiledMap 
{
public:
    ELL_CompiledMap() {}

    ELL_CompiledMap(const ELL_CompiledMap&) = default;

    std::string GetCodeString()
    {
        std::stringstream s;
        if(_map != nullptr)
        {
            _map->WriteCode(s, "asm");
        }
        return s.str();
    }

    std::vector<double> Compute(const std::vector<double>& inputData)
    {
        _map->SetInputValue(0, inputData);
        return _map->ComputeOutput<double>(0);
    }

#ifndef SWIG
    ELL_CompiledMap(const ell::compiler::CompiledMap& other) : _map(std::make_shared<ell::compiler::CompiledMap>(other)) {}
#endif
private:
    std::shared_ptr<ell::compiler::CompiledMap> _map;
};

//
// ELL_OutputPortBaseIterator Methods
//

ELL_OutputPortBaseIterator::ELL_OutputPortBaseIterator() : _i(0), _ports() 
{
}

ELL_OutputPortBaseIterator::~ELL_OutputPortBaseIterator() 
{
}

bool ELL_OutputPortBaseIterator::IsValid() 
{
    return _i < _ports.size();
}

void ELL_OutputPortBaseIterator::Next() 
{
    _i = _i + 1;
}

ELL_OutputPortBase ELL_OutputPortBaseIterator::Get() 
{
    return ELL_OutputPortBase(_ports[_i]);
}

#ifndef SWIG
ELL_OutputPortBaseIterator::ELL_OutputPortBaseIterator(std::vector<ell::model::OutputPortBase*> ports) : 
    _i(0), _ports(ports) 
{
}
#endif

//
// ELL_Node Methods
//

ELL_Node::ELL_Node() 
{
}

ELL_Node::~ELL_Node() 
{
}

#ifndef SWIG
ELL_Node::ELL_Node(const ell::model::Node* other) : 
    _node(other) 
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

ELL_OutputPortBase ELL_Node::GetOutputPort(std::string& portName) 
{
    return ELL_OutputPortBase(_node->GetOutputPort(portName));
}

ELL_InputPortBase ELL_Node::GetInputPort(std::string portName)
{
    return ELL_InputPortBase(_node->GetInputPort(portName));
}

ELL_Port ELL_Node::GetPort(const std::string&  portName) 
{
    return ELL_Port(_node->GetPort(portName));
}

ELL_OutputPortBaseIterator ELL_Node::GetOutputPorts() 
{
    return ELL_OutputPortBaseIterator(_node->GetOutputPorts());
}

ELL_InputPortBaseIterator ELL_Node::GetInputPorts() 
{
    return ELL_InputPortBaseIterator(_node->GetInputPorts());
}

std::string ELL_Node::GetRuntimeTypeName() 
{ 
    return _node->GetRuntimeTypeName(); 
}

//
// ELL_Model Methods
//

ELL_Model::ELL_Model() 
{
}

ELL_Model::ELL_Model(const std::string& filename) : 
    _model(ell::common::LoadModel(filename)) 
{
}

void ELL_Model::Save(const std::string& filename) 
{
    ell::common::SaveModel(_model, filename);
}

size_t ELL_Model::Size() 
{ 
    return _model.Size(); 
}

ELL_NodeIterator ELL_Model::GetNodes() 
{
    ell::model::NodeIterator iter = _model.GetNodeIterator();
    return ELL_NodeIterator(iter);
}

std::string ELL_Model::GetRuntimeTypeName() const
{
    return _model.GetRuntimeTypeName();
}

#ifndef SWIG
ELL_Model::ELL_Model(const ell::model::Model& other) : 
    _model(other) 
{
}
#endif

std::string ELL_Model::GetJson() const
{
    std::stringstream stream;
    ell::utilities::JsonArchiver ar(stream);
    ar << _model;
    return stream.str();
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
    return ELL_Model(model);
}

} // end namespace
%}
