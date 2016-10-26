////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     model.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT
#include "Node.h"
#include "Model.h"
#include "Port.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "PortElements.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "LoadModel.h"
#include "DTWNode.h"
#include "CompiledMap.h"

#include <string>
#include <sstream>
#include <vector>
#include <memory>
%}

#if 0
%nodefaultctor emll::model::NodeIterator;
%nodefaultctor emll::model::Node;
%nodefaultctor emll::model::Port;
%nodefaultctor emll::model::OutputPortBase;
%nodefaultctor emll::model::OutputPort<double>;
%nodefaultctor emll::model::InputPortBase;

%inline %{

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
#ifndef SWIG
    ELL_Node(const emll::model::Node* other);
#endif
private:
    const emll::model::Node* _node;
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
    ELL_InputPortBaseIterator(std::vector<emll::model::InputPortBase*> ports);
#endif
private:
    int _i;
    std::vector<emll::model::InputPortBase*> _ports;
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
    ELL_OutputPortBaseIterator(std::vector<emll::model::OutputPortBase*> ports);
#endif
private:
    int _i;
    std::vector<emll::model::OutputPortBase*> _ports;
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
    ELL_NodeIterator(std::vector<const emll::model::Node*> nodes);
    ELL_NodeIterator(emll::model::NodeIterator& other);
#endif    
private:
    int _i;
    bool _isVector;
    std::vector<const emll::model::Node*> _nodes;
    emll::model::NodeIterator _iterator;
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
#ifndef SWIG
    ELL_Model(const emll::model::Model& other);
#endif
private:
    emll::model::Model _model;
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
    std::string GetTypeName();
    std::string GetRuntimeTypeName();
    int Size();
#ifndef SWIG
    ELL_Port(const emll::model::Port* other);
#endif
private:
    const emll::model::Port* _port;
};

#endif // kirko


%inline %{

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
    ELL_PortElementBase(const emll::model::PortElementBase& other);
#endif
private:
    const emll::model::PortElementBase _port;
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
    std::string GetRuntimeTypeName();
    std::string GetTypeName();
    ELL_NodeIterator GetParentNodes();
#ifndef SWIG
    ELL_InputPortBase(const emll::model::InputPortBase* other);
#endif
private:
    const emll::model::InputPortBase* _port;
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
    void ReferencePort();
    std::string GetTypeName();
    std::string GetRuntimeTypeName();
#ifndef SWIG
    ELL_OutputPortBase(const emll::model::OutputPortBase* other);
#endif
private:
    const emll::model::OutputPortBase* _port;
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
    emll::model::TransformContext _context;
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
ELL_PortElementBase::ELL_PortElementBase(const emll::model::PortElementBase& other) : 
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
// ELL_OutputPortBase Methods 
//

#ifndef SWIG
ELL_OutputPortBase::ELL_OutputPortBase(const emll::model::OutputPortBase* other) : 
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

std::vector<double> ELL_OutputPortBase::GetDoubleOutput() 
{
    return _port->GetDoubleOutput();
}

double ELL_OutputPortBase::GetDoubleOutput(int index) 
{
    return _port->GetDoubleOutput((size_t) index);
}

int ELL_OutputPortBase::Size() 
{
    return (int) _port->Size();
}

void ELL_OutputPortBase::ReferencePort() 
{
    _port->ReferencePort();
}

std::string ELL_OutputPortBase::GetRuntimeTypeName() 
{
    return _port->GetRuntimeTypeName();
}

std::string ELL_OutputPortBase::GetTypeName() 
{
    return _port->GetTypeName();
}

//
// ELL_InputPortBase Methods
//

#ifndef SWIG
ELL_InputPortBase::ELL_InputPortBase(const emll::model::InputPortBase* other) : 
    _port(other) {
}
#endif

ELL_InputPortBase::ELL_InputPortBase() : _port(nullptr) 
{
}

ELL_InputPortBase::~ELL_InputPortBase() 
{
}

int ELL_InputPortBase::Size() 
{
    return  (int) _port->Size();
}

std::string ELL_InputPortBase::GetRuntimeTypeName() 
{
    return _port->GetRuntimeTypeName();
}

std::string ELL_InputPortBase::GetTypeName() 
{
    return _port->GetTypeName();
}

ELL_NodeIterator ELL_InputPortBase::GetParentNodes() 
{
    return ELL_NodeIterator(_port->GetParentNodes());
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

#ifndef SWIG
ELL_Port::ELL_Port(const emll::model::Port* other) : _port(other) 
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

std::string ELL_Port::GetTypeName() 
{
    return _port->GetTypeName();
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
ELL_NodeIterator::ELL_NodeIterator(std::vector<const emll::model::Node*> nodes) : 
    _nodes(nodes) , _i(0), _isVector(true), _iterator() 
{
}

ELL_NodeIterator::ELL_NodeIterator(emll::model::NodeIterator& other) : 
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
ELL_InputPortBaseIterator::ELL_InputPortBaseIterator(std::vector<emll::model::InputPortBase*> ports) : 
    _i(0), _ports(ports) 
{
}
#endif

class ELL_CompiledMap {
public:
    ELL_CompiledMap() {}
    ELL_CompiledMap(const ELL_CompiledMap&) = default;
    std::string CompileToString()
    {
        std::stringstream s;
        if(_map != nullptr)
        {
            _map->WriteCode(s, "asm");
        }
        return s.str();
    }
#ifndef SWIG
    ELL_CompiledMap(const emll::compiler::CompiledMap& other) : _map(std::make_shared<emll::compiler::CompiledMap>(other)) {}
#endif
private:
    std::shared_ptr<emll::compiler::CompiledMap> _map;
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
ELL_OutputPortBaseIterator::ELL_OutputPortBaseIterator(std::vector<emll::model::OutputPortBase*> ports) : 
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
ELL_Node::ELL_Node(const emll::model::Node* other) : 
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

//
// ELL_Model Methods
//

ELL_Model::ELL_Model() 
{
}

ELL_Model::ELL_Model(const std::string& filename) : 
    _model(emll::common::LoadModel(filename)) 
{
}

void ELL_Model::Save(const std::string& filename) 
{
    emll::common::SaveModel(_model, filename);
}

size_t ELL_Model::Size() 
{ 
    return _model.Size(); 
}

ELL_NodeIterator ELL_Model::GetNodes() 
{
    emll::model::NodeIterator iter = _model.GetNodeIterator();
    return ELL_NodeIterator(iter);
}

#ifndef SWIG
ELL_Model::ELL_Model(const emll::model::Model& other) : 
    _model(other) 
{
}
#endif

//
// Functions
//

// TBD

%}
