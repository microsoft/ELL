////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelInterface.h (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

// math
#include "Tensor.h"

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
#include "MathInterface.h"
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
class AutoDataVector;
class CompiledMap;
class Map;
class InputPort;
class Node;
class NodeIterator;
class OutputPort;


//
// PortType
//
enum class PortType : int
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
// Port
//
class Port
{
public:
    Port() = default;
    Node GetNode();
    std::string GetName();
    std::string GetRuntimeTypeName();
    PortType GetOutputType();
    int Size();
#ifndef SWIG
    Port(const ell::model::Port* other);
    const ell::model::Port& GetPort() const { return *_port; }
#endif
private:
    const ell::model::Port* _port = nullptr;
};

//
// InputPortIterator
//

class InputPortIterator
{
public:
    InputPortIterator() = default;
    bool IsValid();
    void Next();
    InputPort Get();
#ifndef SWIG
    InputPortIterator(std::vector<ell::model::InputPortBase*> ports);
#endif
private:
    size_t _i = 0;
    std::vector<ell::model::InputPortBase*> _ports;
};

//
// OutputPortIterator
//

class OutputPortIterator
{
public:
    OutputPortIterator() = default;
    bool IsValid();
    void Next();
    OutputPort Get();
#ifndef SWIG
    OutputPortIterator(std::vector<ell::model::OutputPortBase*> ports);
#endif
private:
    size_t _i = 0;
    std::vector<ell::model::OutputPortBase*> _ports;
};

//
// NodeIterator
//
class NodeIterator
{
public:
    NodeIterator() = default;
    bool IsValid();
    void Next();
    Node Get();
#ifndef SWIG
    NodeIterator(std::vector<const ell::model::Node*> nodes);
    NodeIterator(ell::model::NodeIterator& other);
#endif
private:
    size_t _i = 0;
    bool _isVector = false;
    std::vector<const ell::model::Node*> _nodes;
    ell::model::NodeIterator _iterator;
};

//
// Node
//

class Node
{
public:
    Node() = default;
    Node(const Node&) = default;
    std::string GetId();
    NodeIterator GetParents();
    NodeIterator GetDependents();
    OutputPort GetOutputPort(const std::string& portName);
    InputPort GetInputPort(const std::string& portName);
    Port GetPort(const std::string& portName);
    OutputPortIterator GetOutputPorts();
    InputPortIterator GetInputPorts();
    std::string GetRuntimeTypeName();
#ifndef SWIG
    Node(const ell::model::Node* other);
    const ell::model::Node* GetNode() const { return _node; }
#endif
private:
    const ell::model::Node* _node = nullptr;
};

//
// InputNode
//

class InputNode : public Node
{
public:
    InputNode(const InputNode& node);
    InputNode(Node node);
    using Node::GetInputPort;
    using Node::GetOutputPort;
#ifndef SWIG
    InputNode() = default;
    InputNode(const ell::model::InputNodeBase* other);
    const ell::model::InputNodeBase* GetInputNode() const;
#endif
};

//
// OutputNode
//

class OutputNode : public Node
{
public:
    OutputNode(const OutputNode& node);
    OutputNode(Node node);
    using Node::GetInputPort;
    using Node::GetOutputPort;
#ifndef SWIG
    OutputNode() = default;
    OutputNode(const ell::model::OutputNodeBase* other);
    const ell::model::OutputNodeBase* GetOutputNode() const;
#endif
};

//
// PortElement
//

class PortElement
{
public:
    PortElement() = default;
    int GetIndex();
    PortType GetType();
    OutputPort ReferencedPort();

#ifndef SWIG
    PortElement(const ell::model::PortElementBase& other);
#endif
private:
    ell::model::PortElementBase _port;
};

//
// PortElements
//

class PortElements
{
public:
    PortElements() = default;
    PortElements(const OutputPort& port);
    int Size() const;
    PortType GetType() const;
    PortElement GetElement(int index) const;

#ifndef SWIG
    PortElements(const ell::model::PortElementsBase& other);
    const ell::model::PortElementsBase& GetPortElements() { return _elements; }
#endif
private:
    ell::model::PortElementsBase _elements;
};

//
// InputPort
//

class InputPort
{
public:
    InputPort() = default;
    int Size();
    Node GetNode();
    std::string GetName();
    PortType GetOutputType();
    std::string GetRuntimeTypeName();
    NodeIterator GetParentNodes();
    PortElements GetInputElements();
#ifndef SWIG
    InputPort(const ell::model::InputPortBase* other);
    const ell::model::InputPortBase& GetPort() const { return *_port; }
#endif
private:
    const ell::model::InputPortBase* _port = nullptr;
};

//
// OutputPort
//
class OutputPort
{
public:
    OutputPort() = default;
    int Size();
    Node GetNode();
    std::string GetName();
    PortType GetOutputType();
    void ReferencePort();
    bool IsReferenced() const;
    std::vector<double> GetDoubleOutput();
    double GetDoubleOutput(int index);
#ifndef SWIG
    OutputPort(const ell::model::OutputPortBase* other);
    const ell::model::OutputPortBase& GetPort() const { return *_port; }
#endif
private:
    const ell::model::OutputPortBase* _port = nullptr;
};

//
// Model
//
class Model
{
public:
    Model();
    Model(const std::string& filename);
    void Save(const std::string& filename);
    size_t Size();
    NodeIterator GetNodes();
    std::string GetJson() const;
    Model Refine(int maxIterations);
#ifndef SWIG
    Model(ell::model::Model&& other);
    ell::model::Model& GetModel();
#endif

private:
    std::shared_ptr<ell::model::Model> _model;
};

//
// ModelBuilder
//
class ModelBuilder
{
public:
    ModelBuilder();

    // Generic AddNode method
    Node AddNode(Model model, const std::string& nodeType, const std::vector<std::string>& args);

    // Specific methods per node type
    Node AddDoubleNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<double> predictor);
    Node AddFloatNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<float> predictor);

    Node AddInputNode(Model model, const ell::api::math::TensorShape& shape, PortType type);
    Node AddOutputNode(Model model, const ell::api::math::TensorShape& shape, PortElements input);

    Node AddClockNode(Model model, PortElements input, double interval, short lagThreshold, const std::string& lagNotificationName);
    Node AddSinkNode(Model model, PortElements input, const ell::api::math::TensorShape& shape, const std::string& sinkFunctionName);
    Node AddSourceNode(Model model, PortElements input, PortType outputType, const ell::api::math::TensorShape& shape, const std::string& sourceFunctionName);

private:
#ifndef SWIG
    template <typename ElementType>
    Node AddNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<ElementType> predictor);
#endif

    ell::model::ModelBuilder _modelBuilder;
};

//
// TransformContext
//
class TransformContext
{
public:
#ifndef SWIG
//    TransformContext(const model::NodeActionFunction& isNodeCompilable);
#endif
private:
    ell::model::TransformContext _context;
};

//
// Map
//
class Map
{
public:
    Map();
    Map(const Map& other) = default;
    Map(Model model, InputNode inputNode, PortElements output);
    Map(const std::string& filename);
#ifndef SWIG
    Map(std::shared_ptr<ell::model::DynamicMap>& map);
#endif
    std::vector<double> ComputeDouble(const AutoDataVector& inputData);
    std::vector<double> ComputeDouble(const std::vector<double>& inputData);
    std::vector<float> ComputeFloat(const std::vector<float>& inputData);
    void Save(const std::string& filename) const;
    void Load(const std::string& filename);
    ell::api::math::TensorShape GetInputShape() const;
    ell::api::math::TensorShape GetOutputShape() const;
    CompiledMap Compile(const std::string&  targetDevice, const std::string& moduleName, const std::string& functionName, bool useBlas) const;
    Model GetModel() const;
#ifndef SWIG
    std::shared_ptr<ell::model::DynamicMap> GetInnerMap() { return _map; }
#endif
private:
    std::shared_ptr<ell::model::DynamicMap> _map;
};

//
// CompiledMap
//
class CompiledMap
{
public:
    CompiledMap(const CompiledMap& other) = default;

    void WriteIR(const std::string& filePath);
    void WriteBitcode(const std::string& filePath);
    void WriteSwigInterface(const std::string& filePath);

    std::string GetCodeString();
    std::vector<double> ComputeDouble(const std::vector<double>& inputData);
    std::vector<float> ComputeFloat(const std::vector<float>& inputData);

#ifndef SWIG
    CompiledMap() = default;
    CompiledMap(ell::model::IRCompiledMap map);
#endif

private:
    std::shared_ptr<ell::model::IRCompiledMap> _map;
};


//
// Functions
//

Model LoadModel(std::string filename);
Model LoadModelFromString(std::string str);

} // end namespace
