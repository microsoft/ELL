////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelInterface.h (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

// emitters
#include "EmitterTypes.h"

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
#include "CallbackInterface.h"
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
struct MapCompilerOptions;
struct ModelOptimizerOptions;

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
    void Load(const std::string& filename);
    void LoadFromString(const std::string& str);
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
// Operation types for operation nodes
//
enum class UnaryOperationType
{
    none = (int)ell::emitters::UnaryOperationType::none,
    exp = (int)ell::emitters::UnaryOperationType::exp,
    log = (int)ell::emitters::UnaryOperationType::log,
    sqrt = (int)ell::emitters::UnaryOperationType::sqrt,
    logicalNot = (int)ell::emitters::UnaryOperationType::logicalNot,
    tanh = (int)ell::emitters::UnaryOperationType::tanh,
    square = (int)ell::emitters::UnaryOperationType::square,
    sin = (int)ell::emitters::UnaryOperationType::sin,
    cos = (int)ell::emitters::UnaryOperationType::cos
};

enum class BinaryOperationType
{
    none = (int)ell::emitters::BinaryOperationType::none,
    add = (int)ell::emitters::BinaryOperationType::add,
    subtract = (int)ell::emitters::BinaryOperationType::subtract,
    coordinatewiseMultiply = (int)ell::emitters::BinaryOperationType::coordinatewiseMultiply,
    coordinatewiseDivide = (int)ell::emitters::BinaryOperationType::coordinatewiseDivide,
    logicalAnd = (int)ell::emitters::BinaryOperationType::logicalAnd,
    logicalOr = (int)ell::emitters::BinaryOperationType::logicalOr,
    logicalXor = (int)ell::emitters::BinaryOperationType::logicalXor,
    shiftLeft = (int)ell::emitters::BinaryOperationType::shiftLeft,
    logicalShiftRight = (int)ell::emitters::BinaryOperationType::logicalShiftRight,
    arithmeticShiftRight = (int)ell::emitters::BinaryOperationType::arithmeticShiftRight
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
    Node AddBinaryOperationNode(Model model, PortElements input1, PortElements input2, BinaryOperationType operation);
    Node AddBufferNode(Model model, PortElements input, int windowSize);
    Node AddClockNode(Model model, PortElements input, double interval, double lagThreshold, const std::string& lagNotificationName);
    Node AddConstantNode(Model model, std::vector<double> values, PortType type);
    Node AddDCTNode(Model model, PortElements input, int numFilters);
    Node AddDoubleNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<double> predictor);
    Node AddFFTNode(Model model, PortElements input);
    Node AddFloatNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<float> predictor);
    Node AddHammingWindowNode(Model model, PortElements input);
    Node AddIIRFilterNode(Model model, PortElements input, std::vector<double> bCoeffs, std::vector<double> aCoeffs);
    InputNode AddInputNode(Model model, const ell::api::math::TensorShape& shape, PortType type);
    Node AddLinearFilterBankNode(Model model, PortElements input, double sampleRate, int numFilters, int numFiltersToUse);
    Node AddMelFilterBankNode(Model model, PortElements input, double sampleRate, int numFilters, int numFiltersToUse);
    OutputNode AddOutputNode(Model model, const ell::api::math::TensorShape& shape, PortElements input);
    Node AddSinkNode(Model model, PortElements input, PortElements trigger, const ell::api::math::TensorShape& shape, const std::string& sinkFunctionName);
    Node AddSourceNode(Model model, PortElements input, PortType outputType, const ell::api::math::TensorShape& shape, const std::string& sourceFunctionName);
    Node AddUnaryOperationNode(Model model, PortElements input, UnaryOperationType operation);

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
    Map(std::shared_ptr<ell::model::Map>& map);
#endif

    void Save(const std::string& filename) const;
    void Load(const std::string& filename);
    ell::api::math::TensorShape GetInputShape() const;
    ell::api::math::TensorShape GetOutputShape() const;
    Model GetModel() const;

    // Note: not templatized because these implement type-specific resolverFunctions
    CompiledMap CompileDouble(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName, const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings) const;
    CompiledMap CompileFloat(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName, const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings) const;

    // Return true if the model contains a SourceNode.  In this case you need 
    // to register the callbacks via SetSourceCallback and SetSinkCallback.
    bool HasSourceNodes();

    template <typename ElementType>
    void SetSourceCallback(ell::api::CallbackBase<ElementType>& callback, size_t index);
    template <typename ElementType>
    void SetSinkCallback(ell::api::CallbackBase<ElementType>& callback, size_t index);
    template <typename ElementType>
    void Step(ell::api::TimeTickType timestamp = 0.0);

    // Older non callback based API, only makes sense when model has single input/output nodes and no source/sink nodes.
    std::vector<double> ComputeDouble(const AutoDataVector& inputData);
    std::vector<double> ComputeDouble(const std::vector<double>& inputData);
    std::vector<float> ComputeFloat(const std::vector<float>& inputData);

#ifndef SWIG
    std::shared_ptr<ell::model::Map> GetInnerMap() { return _map; }
#endif

private:
#ifndef SWIG
    CompiledMap Compile(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName,
                        const std::string& sourceFunctionName, const std::string& sinkFunctionName,
                        const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings,
                        std::function<void(llvm::Module*, ell::emitters::IRExecutionEngine&)> resolverFunction) const;
#endif

    std::shared_ptr<ell::model::Map> _map;
    int _hasSourceNodes = 0;
};

//
// CompiledMap
//
class CompiledMap
{
public:
    ~CompiledMap();

    void WriteIR(const std::string& filePath);
    void WriteBitcode(const std::string& filePath);
    void WriteSwigInterface(const std::string& filePath);
    std::string GetCodeString();

    template <typename ElementType>
    void RegisterCallbacks(ell::api::CallbackBase<ElementType>& inputCallback, ell::api::CallbackBase<ElementType>& outputCallback);

    template <typename ElementType>
    void Step(ell::api::TimeTickType timestamp = 0.0);

    template <typename ElementType>
    void UnregisterCallbacks();

#ifndef SWIG
    CompiledMap() = default;
    CompiledMap(ell::model::IRCompiledMap map, ell::api::math::TensorShape inputShape, ell::api::math::TensorShape outputShape);

    template <typename ElementType>
    static bool InvokeSourceCallback(ElementType* input);

    template <typename ElementType>
    static void InvokeSinkCallback(ElementType* output);
#endif

private:
    template <typename ElementType>
    static ell::api::CallbackForwarder<ElementType, ElementType>& CallbackForwarder();

    std::shared_ptr<ell::model::IRCompiledMap> _map;
    ell::api::math::TensorShape _inputShape;
    ell::api::math::TensorShape _outputShape;
};

//
// Compiler options
//
struct MapCompilerOptions
{
    bool useBlas = true;
    bool profile = false;
};

//
// Optimizer settings
//
struct ModelOptimizerOptions
{
    bool fuseLinearFunctionNodes = true;
};

} // end namespace

#include "../tcc/ModelInterface.tcc"
