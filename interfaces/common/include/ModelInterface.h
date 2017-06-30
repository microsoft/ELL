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

// apis
#include "NeuralNetworkPredictorInterface.h"

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
class ELL_CompiledMap;
class ELL_DynamicMap;
class ELL_InputPort;
class ELL_Node;
class ELL_NodeIterator;
class ELL_OutputPort;
class ELL_SteppableMap;

//
// ELL_ClockType
//
enum class ELL_ClockType
{
    steadyClock,
    systemClock
};

//
// ELL_PortType
//
enum class ELL_PortType
{
    none = (int)ell::model::Port::PortType::none,
    smallReal = (int)ell::model::Port::PortType::smallReal, // == float
    real = (int)ell::model::Port::PortType::real, // == double
    integer = (int)ell::model::Port::PortType::integer, // == int32
    bigInt = (int)ell::model::Port::PortType::bigInt, // == int64
    categorical = (int)ell::model::Port::PortType::categorical,
    boolean = (int)ell::model::Port::PortType::boolean
};

//
// ELL_Port
//
class ELL_Port
{
public:
    ELL_Port() = default;
    ELL_Node GetNode();
    std::string GetName();
    std::string GetRuntimeTypeName();
    ELL_PortType GetOutputType();
    int Size();
#ifndef SWIG
    ELL_Port(const ell::model::Port* other);
    const ell::model::Port& GetPort() const { return *_port; }
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
    const ell::model::Node* GetNode() const { return _node; }
#endif
private:
    const ell::model::Node* _node = nullptr;
};

//
// ELL_InputNode
//

class ELL_InputNode : public ELL_Node
{
public:
    ELL_InputNode(const ELL_InputNode& node);
    ELL_InputNode(ELL_Node node);
    using ELL_Node::GetInputPort;
    using ELL_Node::GetOutputPort;
#ifndef SWIG
    ELL_InputNode() = default;
    ELL_InputNode(const ell::model::InputNodeBase* other);
    const ell::model::InputNodeBase* GetInputNode() const;
#endif
};

//
// ELL_OutputNode
//

class ELL_OutputNode : public ELL_Node
{
public:
    ELL_OutputNode(const ELL_OutputNode& node);
    ELL_OutputNode(ELL_Node node);
    using ELL_Node::GetInputPort;
    using ELL_Node::GetOutputPort;
#ifndef SWIG
    ELL_OutputNode() = default;
    ELL_OutputNode(const ell::model::OutputNodeBase* other);
    const ell::model::OutputNodeBase* GetOutputNode() const;
#endif
};

//
// ELL_PortElement
//

class ELL_PortElement
{
public:
    ELL_PortElement() = default;
    int GetIndex();
    ELL_PortType GetType();
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
    ELL_PortElements(const ELL_OutputPort& port);
    int Size() const;
    ELL_PortType GetType() const;
    ELL_PortElement GetElement(int index) const;

#ifndef SWIG
    ELL_PortElements(const ell::model::PortElementsBase& other);
    const ell::model::PortElementsBase& GetPortElements() { return _elements; }
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
    ELL_PortType GetOutputType();
    std::string GetRuntimeTypeName();
    ELL_NodeIterator GetParentNodes();
    ELL_PortElements GetInputElements();
#ifndef SWIG
    ELL_InputPort(const ell::model::InputPortBase* other);
    const ell::model::InputPortBase& GetPort() const { return *_port; }
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
    ELL_PortType GetOutputType();
    void ReferencePort();
    bool IsReferenced() const;
    std::vector<double> GetDoubleOutput();
    double GetDoubleOutput(int index);
#ifndef SWIG
    ELL_OutputPort(const ell::model::OutputPortBase* other);
    const ell::model::OutputPortBase& GetPort() const { return *_port; }
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

    // Generic AddNode method
    ELL_Node AddNode(ELL_Model model, const std::string& nodeType, const std::vector<std::string>& args);

    // Specific methods per node type
    ELL_Node AddDoubleNeuralNetworkPredictorNode(ELL_Model model, ELL_PortElements input, ell::api::predictors::NeuralNetworkPredictor<double> predictor);
    ELL_Node AddFloatNeuralNetworkPredictorNode(ELL_Model model, ELL_PortElements input, ell::api::predictors::NeuralNetworkPredictor<float> predictor);

    ELL_Node AddInputNode(ELL_Model model, int size, ELL_PortType type);
    ELL_Node AddOutputNode(ELL_Model model, ELL_PortElements input);
    ELL_Node AddSinkNode(ELL_Model model, ELL_PortElements input, const std::string& sinkFunctionName);
    ELL_Node AddSourceNode(ELL_Model model, ELL_PortElements input, ELL_PortType outputType, int outputSize, const std::string& sourceFunctionName);

private:
#ifndef SWIG
    template <typename ElementType>
    ELL_Node AddNeuralNetworkPredictorNode(ELL_Model model, ELL_PortElements input, ell::api::predictors::NeuralNetworkPredictor<ElementType> predictor);
#endif

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
// ELL_Map
//
class ELL_Map
{
public:
    ELL_Map(const ELL_Map& other) = default;
    ELL_Map(ELL_Model model, ELL_InputNode inputNode, ELL_PortElements output);
    std::vector<double> ComputeDouble(const std::vector<double>& inputData);
    std::vector<float> ComputeFloat(const std::vector<float>& inputData);
    void Save(const std::string& filePath) const;
    
private:
    std::shared_ptr<ell::model::DynamicMap> _map;
};

//
// ELL_SteppableMap
//
class ELL_SteppableMap
{
public:
    ELL_SteppableMap(const ELL_SteppableMap& other) = default;
    ELL_SteppableMap(ELL_Model model, ELL_InputNode inputNode, ELL_PortElements output, ELL_ClockType clockType, int millisecondInterval);
    void Save(const std::string& filePath) const;

private:
    std::shared_ptr<ell::model::DynamicMap> _map;
    ELL_ClockType _clockType;
};

//
// ELL_CompiledMap
//
class ELL_CompiledMap
{
public:
    ELL_CompiledMap(const ELL_CompiledMap& other) = default;

    std::string GetCodeString();
    std::vector<double> ComputeDouble(const std::vector<double>& inputData);
    std::vector<float> ComputeFloat(const std::vector<float>& inputData);

#ifndef SWIG
    ELL_CompiledMap() = default;
    ELL_CompiledMap(ell::model::IRCompiledMap map);
#endif

private:
    std::shared_ptr<ell::model::IRCompiledMap> _map;
};

//
// Functions
//

ELL_Model ELL_LoadModel(std::string filename);
ELL_Model ELL_LoadModelFromString(std::string str);

} // end namespace
