////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelInterface.h (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

#include "CallbackInterface.h"
#include "MathInterface.h"

#include <emitters/include/EmitterTypes.h>

#include <math/include/Tensor.h>

#include <model/include/IRCompiledMap.h>
#include <model/include/InputNode.h>
#include <model/include/InputPort.h>
#include <model/include/Model.h>
#include <model/include/Node.h>
#include <model/include/OutputNode.h>
#include <model/include/OutputPort.h>
#include <model/include/Port.h>
#include <model/include/PortElements.h>
#include <model/include/PortMemoryLayout.h>

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
class PortMemoryLayout;
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
    PortMemoryLayout GetMemoryLayout();

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
    InputPortIterator(const std::vector<const ell::model::InputPortBase*>& ports);
#endif
private:
    size_t _i = 0;
    std::vector<const ell::model::InputPortBase*> _ports;
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
    OutputPortIterator(const std::vector<const ell::model::OutputPortBase*>& ports);
#endif
private:
    size_t _i = 0;
    std::vector<const ell::model::OutputPortBase*> _ports;
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
    NodeIterator(const std::vector<const ell::model::Node*>& nodes);
    NodeIterator(ell::model::ForwardNodeIterator& other);
#endif
private:
    size_t _i = 0;
    bool _isVector = false;
    std::vector<const ell::model::Node*> _nodes;
    ell::model::ForwardNodeIterator _iterator;
};

//
// Port Memory Layout
//
class PortMemoryLayout
{
public:
    const std::vector<int> size;
    const std::vector<int> padding;
    const std::vector<int> offset;
    const std::vector<int> order;

    PortMemoryLayout(const std::vector<int>& size,
                     const std::vector<int>& padding = {},
                     const std::vector<int>& offset = {},
                     const std::vector<int>& order = {});

    bool IsEqual(const PortMemoryLayout& other);

#ifndef SWIG
    const ell::model::PortMemoryLayout& Get() const
    {
        return _layout;
    }

    PortMemoryLayout(const ell::model::PortMemoryLayout& layout);

#endif
private:
    ell::model::PortMemoryLayout _layout;
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
    std::string GetMetadataValue(const std::string& key);
    void SetMetadataValue(const std::string& key, const std::string& value);
    void CopyMetadataFrom(const Node& other);
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
    PortMemoryLayout GetMemoryLayout() const;
    PortType GetType() const;
    PortElement GetElement(int index) const;

#ifndef SWIG
    PortElements(const ell::model::PortElementsBase& other);
    PortElements(const ell::model::OutputPortBase& port);
    const ell::model::PortElementsBase& GetPortElements() const { return _elements; }
#endif
private:
    ell::model::PortElementsBase _elements;
};

//
// InputPort
//

class InputPort : public Port
{
public:
    InputPort() = default;
    NodeIterator GetParentNodes();
    OutputPort GetReferencedPort();
#ifndef SWIG
    InputPort(const ell::model::InputPortBase* other);
    const ell::model::InputPortBase& GetInputPort() const { return *_input_port; }
#endif
private:
    const ell::model::InputPortBase* _input_port = nullptr;
};

//
// OutputPort
//
class OutputPort : public Port
{
public:
    OutputPort() = default;
    bool IsReferenced() const;
    InputPortIterator GetReferences();
    std::vector<double> GetDoubleOutput();
    double GetDoubleOutput(int index);
#ifndef SWIG
    OutputPort(const ell::model::OutputPortBase* other);
    const ell::model::OutputPortBase& GetOutputPort() const { return *_output_port; }
#endif
private:
    const ell::model::OutputPortBase* _output_port = nullptr;
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
// Map
//
class Map
{
public:
    Map();
    Map(const Map& other) = default;
    Map(Model model, InputNode inputNode, PortElements output);
    Map(Model model, const std::vector<InputNode*> inputNodes, const std::vector<PortElements*> outputs);
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

    void Reset();

    // Older non callback based API, only makes sense when model has single input/output nodes and no source/sink nodes.
    std::vector<double> ComputeDouble(const AutoDataVector& inputData);
    std::vector<double> ComputeDouble(const std::vector<double>& inputData);
    std::vector<float> ComputeFloat(const std::vector<float>& inputData);

#ifndef SWIG
    std::shared_ptr<ell::model::Map> GetInnerMap()
    {
        return _map;
    }
#endif

private:
#ifndef SWIG
    CompiledMap Compile(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName, const std::string& sourceFunctionName, const std::string& sinkFunctionName, const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings, std::function<void(llvm::Module*, ell::emitters::IRExecutionEngine&)> resolverFunction) const;
#endif

    std::shared_ptr<ell::model::Map> _map;
    enum class TriState
    {
        Uninitialized,
        No,
        Yes
    };
    TriState _sourceNodeState = TriState::Uninitialized;
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
    CompiledMap(
        std::shared_ptr<ell::model::IRMapCompiler> compiler,
        std::shared_ptr<ell::model::IRCompiledMap> map,
        ell::api::math::TensorShape inputShape,
        ell::api::math::TensorShape outputShape);

    template <typename ElementType>
    bool InvokeSourceCallback(ElementType* input);

    template <typename ElementType>
    void InvokeSinkCallback(ElementType* output);
#endif

    // Return true if the model contains a SourceNode.  In this case you need
    // to register the callbacks via SetSourceCallback and SetSinkCallback.
    bool HasSourceNodes();

    // Older non callback based API, only makes sense when model has single input/output nodes and no source/sink nodes.
    std::vector<double> ComputeDouble(const std::vector<double>& inputData);
    std::vector<float> ComputeFloat(const std::vector<float>& inputData);

private:
    template <typename ElementType>
    ell::api::CallbackForwarder<ElementType, ElementType>& GetCallbackForwarder();

    std::shared_ptr<ell::model::IRMapCompiler> _compiler;
    std::shared_ptr<ell::model::IRCompiledMap> _map;
    ell::api::math::TensorShape _inputShape;
    ell::api::math::TensorShape _outputShape;
    ell::api::CallbackForwarder<double, double> forwarderDouble;
    ell::api::CallbackForwarder<float, float> forwarderFloat;

    enum class TriState
    {
        Uninitialized,
        No,
        Yes
    };
    TriState _sourceNodeState = TriState::Uninitialized;
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

} // namespace ELL_API

#pragma region implementation

#ifndef SWIG

#include <model/include/IRMapCompiler.h>

#include <nodes/include/SinkNode.h>
#include <nodes/include/SourceNode.h>

namespace ELL_API
{

//
// Map
//
template <typename ElementType>
void Map::SetSourceCallback(ell::api::CallbackBase<ElementType>& callback, size_t index)
{
    auto nodes = GetModel().GetModel().GetNodesByType<ell::nodes::SourceNode<ElementType>>();
    if (nodes.size() == 0)
    {
        std::string name = typeid(ElementType).name();
        throw std::invalid_argument("Cannot set SourceCallback because model has no SourceNode of type '" + name + "'");
    }
    nodes.at(index)->SetSourceFunction([&callback](auto& input) {
        return callback.Run(input);
    });
}

template <typename ElementType>
void Map::SetSinkCallback(ell::api::CallbackBase<ElementType>& callback, size_t index)
{
    auto nodes = GetModel().GetModel().GetNodesByType<ell::nodes::SinkNode<ElementType>>();

    if (nodes.size() == 0)
    {
        std::string name = typeid(ElementType).name();
        throw std::invalid_argument("Cannot set SinkCallback because model has no SinkNode of type '" + name + "'");
    }

    nodes.at(index)->SetSinkFunction([&callback](const std::vector<ElementType>& output) {
        // Reason for the const_cast:
        // SWIG warns that the const overload gets shadowed, so CallbackBase only
        // provides a non-const vector overload for Run.
        callback.Run(const_cast<std::vector<ElementType>&>(output));
    });
}

template <typename ElementType>
void Map::Step(ell::api::TimeTickType timestamp)
{
    std::vector<ell::api::TimeTickType> input = { timestamp };
    _map->Compute<ElementType>(input);
}

//
// CompiledMap
//
template <typename ElementType>
void CompiledMap::Step(ell::api::TimeTickType timestamp)
{
    // Note: casting TimeTickType to match input and output port types
    std::vector<ell::api::TimeTickType> input = { timestamp };
    _map->SetContext(this);
    _map->Compute<ElementType>(input);
}

template <typename ElementType>
void CompiledMap::RegisterCallbacks(
    ell::api::CallbackBase<ElementType>& inputCallback,
    ell::api::CallbackBase<ElementType>& outputCallback)
{
    ell::api::CallbackBase<ell::api::TimeTickType> unusedLagCallback;
    GetCallbackForwarder<ElementType>().Register(inputCallback, _inputShape.Size(), outputCallback, _outputShape.Size(), unusedLagCallback);
}

template <typename ElementType>
void CompiledMap::UnregisterCallbacks()
{
    GetCallbackForwarder<ElementType>().Clear();
}

template <typename ElementType>
bool CompiledMap::InvokeSourceCallback(ElementType* input)
{
    return GetCallbackForwarder<ElementType>().InvokeInput(input);
}

template <typename ElementType>
void CompiledMap::InvokeSinkCallback(ElementType* output)
{
    GetCallbackForwarder<ElementType>().InvokeOutput(output);
}

} // namespace ELL_API

#endif // SWIG

#pragma endregion implementation
