////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelInterface.h (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

// model
#include "IRCompiledMap.h"
#include "InputNode.h"
#include "InputPort.h"
#include "Model.h"
#include "ModelBuilder.h"
#include "Node.h"
#include "OutputNode.h"
#include "OutputPort.h"
#include "Port.h"
#include "PortElements.h"

// stl
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#endif

namespace ELL_API
{

//
// Forward declarations
//
class ELL_Node;
class ELL_NodeIterator;
class ELL_InputPort;
class ELL_OutputPort;

//
// ELL_Port 
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
    ELL_Port() = default;
    ELL_Node GetNode();
    std::string GetName();
    std::string GetRuntimeTypeName();
    int GetOutputType();
    int Size();
#ifndef SWIG
    ELL_Port(const ell::model::Port* other);
#endif
private:
    const ell::model::Port* _port = nullptr;
};

//
// ELL_InputPortIterator 
//

class ELL_InputPortIterator
{
public:
    ELL_InputPortIterator() = default;
    bool IsValid();
    void Next();
    ELL_InputPort Get();
#ifndef SWIG
    ELL_InputPortIterator(std::vector<ell::model::InputPortBase*> ports);
#endif
private:
    size_t _i = 0;
    std::vector<ell::model::InputPortBase*> _ports;
};

//
// ELL_OutputPortIterator 
//

class ELL_OutputPortIterator
{
public:
    ELL_OutputPortIterator() = default;
    bool IsValid();
    void Next();
    ELL_OutputPort Get();
#ifndef SWIG
    ELL_OutputPortIterator(std::vector<ell::model::OutputPortBase*> ports);
#endif
private:
    size_t _i = 0;
    std::vector<ell::model::OutputPortBase*> _ports;
};

//
// ELL_NodeIterator 
//
class ELL_NodeIterator
{
public:
    ELL_NodeIterator() = default;
    bool IsValid();
    void Next();
    ELL_Node Get();
#ifndef SWIG
    ELL_NodeIterator(std::vector<const ell::model::Node*> nodes);
    ELL_NodeIterator(ell::model::NodeIterator& other);
#endif
private:
    size_t _i = 0;
    bool _isVector = false;
    std::vector<const ell::model::Node*> _nodes;
    ell::model::NodeIterator _iterator;
};

//
// ELL_Node 
//

class ELL_Node
{
public:
    ELL_Node() = default;
    ELL_Node(const ELL_Node&) = default;
    std::string GetId();
    ELL_NodeIterator GetParents();
    ELL_NodeIterator GetDependents();
    ELL_OutputPort GetOutputPort(const std::string& portName);
    ELL_InputPort GetInputPort(const std::string& portName);
    ELL_Port GetPort(const std::string& portName);
    ELL_OutputPortIterator GetOutputPorts();
    ELL_InputPortIterator GetInputPorts();
    std::string GetRuntimeTypeName();
#ifndef SWIG
    ELL_Node(const ell::model::Node* other);
#endif
private:
    const ell::model::Node* _node = nullptr;
};

//
// ELL_PortElement 
//

class ELL_PortElement
{
public:
    ELL_PortElement() = default;
    int GetIndex();
    int GetType();
    ELL_OutputPort ReferencedPort();

#ifndef SWIG
    ELL_PortElement(const ell::model::PortElementBase& other);
#endif
private:
    ell::model::PortElementBase _port;
};

//
// ELL_PortElements 
//

class ELL_PortElements
{
public:
    ELL_PortElements() = default;
    int Size() const;
    int GetType() const;
    ELL_PortElement GetElement(int index) const;

#ifndef SWIG
    ELL_PortElements(const ell::model::PortElementsBase& other);
#endif
private:
    ell::model::PortElementsBase _elements;
};

//
// ELL_InputPort 
//

class ELL_InputPort
{
public:
    ELL_InputPort() = default;
    int Size();
    ELL_Node GetNode();
    std::string GetName();
    int GetOutputType();
    std::string GetRuntimeTypeName();
    ELL_NodeIterator GetParentNodes();
    ELL_PortElements GetInputElements();
#ifndef SWIG
    ELL_InputPort(const ell::model::InputPortBase* other);
#endif
private:
    const ell::model::InputPortBase* _port = nullptr;
};

//
// ELL_OutputPort 
//
class ELL_OutputPort
{
public:
    ELL_OutputPort() = default;
    int Size();
    ELL_Node GetNode();
    std::string GetName();
    int GetOutputType();
    void ReferencePort();
    bool IsReferenced() const;
    std::vector<double> GetDoubleOutput();
    double GetDoubleOutput(int index);
#ifndef SWIG
    ELL_OutputPort(const ell::model::OutputPortBase* other);
#endif
private:
    const ell::model::OutputPortBase* _port = nullptr;
};

//
// ELL_Model 
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
#ifndef SWIG
    ELL_Model(ell::model::Model&& other);
    ell::model::Model& GetModel();
#endif

private:
    std::shared_ptr<ell::model::Model> _model;
};

//
// ELL_ModelBuilder 
//
class ELL_ModelBuilder
{
public:
    ELL_ModelBuilder();
    ELL_Node AddNode(ELL_Model model, const std::string& nodeType, const std::vector<std::string>& args);

private:
    ell::model::ModelBuilder _modelBuilder;
};

//
// ELL_TransformContext 
//
class ELL_TransformContext
{
public:
#ifndef SWIG
//    ELL_TransformContext(const model::NodeActionFunction& isNodeCompilable);
#endif
private:
    ell::model::TransformContext _context;
};

//
// ELL_DynamicMap 
//
class ELL_DynamicMap
{
public:
    std::vector<double> Compute(const std::vector<double>& inputData);

private:
    std::shared_ptr<ell::model::DynamicMap> _map;
};

//
// ELL_CompiledMap 
//
class ELL_CompiledMap
{
public:
    std::string GetCodeString();
    std::vector<double> Compute(const std::vector<double>& inputData);

private:
    std::shared_ptr<ell::model::IRCompiledMap> _map;
};

//
// Functions
//

ELL_Model ELL_LoadModel(std::string filename);
ELL_Model ELL_LoadModelFromString(std::string str);

} // end namespace
