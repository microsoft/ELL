////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DatasetInterface.h"
#include "DatasetInterfaceImpl.h"
#include "ModelInterface.h"

// common
#include "LoadModel.h"
#include "RegisterNodeCreators.h"

// emitters
#include "ModuleEmitter.h"

// utilities
#include "JsonArchiver.h"
#include "StringUtil.h"

// math
#include "DenseDataVector.h"
#include "FilterBank.h"

// model
#include "InputNode.h"
#include "Map.h"
#include "MapLoadArguments.h"
#include "OutputNode.h"

// stl
#include <algorithm>

//
// Callback functions
//

#ifdef __cplusplus
extern "C" {
#endif

// Note: this currently assumes that there is just 1 source and 1 sink in the map
// Future: extend this to route to multiple sources + sinks based on extra context parameter.
bool model_CompiledMap_SourceCallback_Double(void* context, double* input)
{
    // Note: this context is passed through by IRCompiledMap::SetNodeInput where it calls GetContext()
    // for the first parameter to the compiled _computeInputFunction and SetContext() happens in
    // CompiledMap::Step so we know the context is the CompiledMap interface object.
    auto map = reinterpret_cast<ELL_API::CompiledMap*>(context);
    return map->InvokeSourceCallback(input);
}

bool model_CompiledMap_SourceCallback_Float(void* context, float* input)
{
    auto map = reinterpret_cast<ELL_API::CompiledMap*>(context);
    return map->InvokeSourceCallback(input);
}

void model_CompiledMap_SinkCallback_Double(void* context, double* output)
{
    auto map = reinterpret_cast<ELL_API::CompiledMap*>(context);
    return map->InvokeSinkCallback(output);
}

void model_CompiledMap_SinkCallback_Float(void* context, float* output)
{
    auto map = reinterpret_cast<ELL_API::CompiledMap*>(context);
    return map->InvokeSinkCallback(output);
}

#ifdef __cplusplus
} // extern "C"
#endif

using namespace ell::utilities;

namespace ELL_API
{

//
// Port
//
PortType Port::GetOutputType()
{
    return static_cast<PortType>(_port->GetType());
}

Port::Port(const ell::model::Port* other)
    : _port(other)
{
}

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

InputPortIterator::InputPortIterator(std::vector<ell::model::InputPortBase*> ports)
    : _i(0), _ports(ports)
{
}

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

OutputPortIterator::OutputPortIterator(std::vector<ell::model::OutputPortBase*> ports)
    : _i(0), _ports(ports)
{
}

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

NodeIterator::NodeIterator(std::vector<const ell::model::Node*> nodes)
    : _i(0), _isVector(true), _nodes(nodes), _iterator()
{
}

NodeIterator::NodeIterator(ell::model::NodeIterator& other)
    : _i(0), _isVector(false), _nodes(0), _iterator(other)
{
}

//
// Node
//
Node::Node(const ell::model::Node* other)
    : _node(other)
{
}

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
    using namespace std::string_literals;
    auto port = _node->GetInputPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '"s + portName + "'");
    }
    return InputPort(port);
}

Port Node::GetPort(const std::string& portName)
{
    using namespace std::string_literals;
    auto port = _node->GetPort(portName);
    if (port == nullptr)
    {
        throw std::invalid_argument("no port named '"s + portName + "'");
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

std::string Node::GetMetadataValue(const std::string& key)
{
    std::string value;
    if (_node->GetMetadata().HasEntry(key))
    {
        value = _node->GetMetadata().GetEntry<std::string>(key);
    }
    return value;
}

void Node::SetMetadataValue(const std::string& key, const std::string& value)
{
    auto node = const_cast<ell::model::Node*>(_node);
    node->GetMetadata()[key] = value;
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
PortElement::PortElement(const ell::model::PortElementBase& other)
    : _port(other)
{
}

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
        throw Exception("no referenced port");
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

PortMemoryLayout PortElements::GetMemoryLayout() const
{
    return _elements.GetMemoryLayout();
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

InputPort::InputPort(const ell::model::InputPortBase* other)
    : _port(other)
{
}

PortType InputPort::GetOutputType()
{
    return static_cast<PortType>(_port->GetType());
}

Node InputPort::GetNode()
{
    return Node(_port->GetNode());
}

int InputPort::Size() const
{
    return (int)_port->Size();
}

PortMemoryLayout InputPort::GetMemoryLayout() const
{
    return { _port->GetMemoryLayout() };
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
OutputPort::OutputPort(const ell::model::OutputPortBase* other)
    : _port(other)
{
}

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

int OutputPort::Size() const
{
    return (int)_port->Size();
}

PortMemoryLayout OutputPort::GetMemoryLayout() const
{
    return { _port->GetMemoryLayout() };
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
// PortMemoryLayout
//
PortMemoryLayout::PortMemoryLayout(const std::vector<int>& s, const std::vector<int>& p, const std::vector<int>& o, const std::vector<int>& order)
    : size(s), padding(p), offset(o), order(order)
{
    if (padding.size() == 0 && offset.size() == 0)
    {
        _layout = ell::model::PortMemoryLayout(ell::model::MemoryShape{ size });
    }
    else if (offset.size() == 0)
    {
        _layout = ell::model::PortMemoryLayout(ell::model::MemoryShape{ size }, ell::model::MemoryShape{ padding });
    }
    else if (order.size() == 0)
    {
        _layout = ell::model::PortMemoryLayout(ell::model::MemoryShape{ size }, ell::model::MemoryShape{ padding }, ell::model::MemoryShape{ offset });
    }
    else
    {
        _layout = ell::model::PortMemoryLayout(ell::model::MemoryShape{ size }, ell::model::MemoryShape{ padding }, ell::model::MemoryShape{ offset }, ell::model::DimensionOrder{ order });
    }
}

PortMemoryLayout::PortMemoryLayout(const ell::model::PortMemoryLayout& layout)
    : _layout(layout)
{
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
    Load(filename);
}

void Model::Load(const std::string& filename)
{
    _model = std::make_shared<ell::model::Model>(ell::common::LoadModel(filename));
}

void Model::Save(const std::string& filename)
{
    ell::common::SaveModel(*_model, filename);
}

void Model::LoadFromString(const std::string& str)
{
    _model = std::make_shared<ell::model::Model>();
    std::stringstream stream(str);
    SerializationContext context;
    JsonUnarchiver ar(stream, context);
    ar >> *_model;
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
    JsonArchiver ar(stream);
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

Model::Model(ell::model::Model&& other)
{
    _model = std::make_shared<ell::model::Model>(std::move(other));
}

ell::model::Model& Model::GetModel()
{
    return *_model;
}

//
// Map
//
Map::Map()
{
    _map = std::make_shared<ell::model::Map>();
}

Map::Map(Model model, InputNode inputNode, PortElements output)
{
    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> inputs = { std::pair<std::string, ell::model::InputNodeBase*>{ "input", const_cast<ell::model::InputNodeBase*>(inputNode.GetInputNode()) } };
    auto outputs = std::vector<std::pair<std::string, ell::model::PortElementsBase>>{ { "output", output.GetPortElements() } };
    _map = std::make_shared<ell::model::Map>(model.GetModel(), inputs, outputs);
}

Map::Map(std::shared_ptr<ell::model::Map>& map)
    : _map(map)
{
}

Map::Map(const std::string& filename)
{
    Load(filename);
}

ell::api::math::TensorShape Map::GetInputShape() const
{
    return ell::api::math::TensorShape::FromMemoryShape(_map->GetInputShape());
}

ell::api::math::TensorShape Map::GetOutputShape() const
{
    return ell::api::math::TensorShape::FromMemoryShape(_map->GetOutputShape());
}

Model Map::GetModel() const
{
    ell::model::Model model = _map->GetModel();
    return Model(std::move(model));
}

void Map::Load(const std::string& filename)
{
    ell::common::MapLoadArguments args;
    args.inputMapFilename = filename;
    _map = std::make_shared<ell::model::Map>(ell::common::LoadMap(args));
    _sourceNodeState = TriState::Uninitialized;
}

void Map::Save(const std::string& filename) const
{
    ell::common::SaveMap(*_map, filename);
}

void Map::Reset()
{
    _map->Reset();
}

bool Map::HasSourceNodes()
{
    if (_sourceNodeState == TriState::Uninitialized)
    {
        // lazily search for SourceNode and cache the answer so next call is much faster.
        auto sourceNodes = _map->GetModel().GetNodesByType<ell::model::SourceNodeBase>();
        _sourceNodeState = sourceNodes.empty() ? TriState::No : TriState::Yes;
    }
    return _sourceNodeState == TriState::Yes;
}

std::vector<double> Map::ComputeDouble(const AutoDataVector& inputData)
{
    const ell::data::AutoDataVector& data = *(inputData._impl->_vector);
    ell::data::DenseDataVector<double> output = _map->Compute<ell::data::DenseDataVector<double>>(data);
    return output.ToArray();
}

std::vector<double> Map::ComputeDouble(const std::vector<double>& inputData)
{
    return _map->Compute<double>(inputData);
}

std::vector<float> Map::ComputeFloat(const std::vector<float>& inputData)
{
    return _map->Compute<float>(inputData);
}

CompiledMap Map::CompileDouble(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName, const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings) const
{
    auto resolverFunction = [moduleName](llvm::Module* module, ell::emitters::IRExecutionEngine& jitter) {
        auto func = module->getFunction(moduleName + "_CompiledMap_SourceCallback_Double");
        if (func != nullptr)
        {
            jitter.DefineFunction(func, reinterpret_cast<uint64_t>(&model_CompiledMap_SourceCallback_Double));
        }

        func = module->getFunction(moduleName + "_CompiledMap_SinkCallback_Double");
        if (func != nullptr)
        {
            jitter.DefineFunction(func, reinterpret_cast<uint64_t>(&model_CompiledMap_SinkCallback_Double));
        }
    };

    return Map::Compile(targetDevice, moduleName, functionName, "CompiledMap_SourceCallback_Double", "CompiledMap_SinkCallback_Double", compilerSettings, optimizerSettings, resolverFunction);
}

CompiledMap Map::CompileFloat(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName, const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings) const
{
    auto resolverFunction = [moduleName](llvm::Module* module, ell::emitters::IRExecutionEngine& jitter) {
        auto func = module->getFunction(moduleName + "_CompiledMap_SourceCallback_Float");
        if (func != nullptr)
        {
            jitter.DefineFunction(func, reinterpret_cast<uint64_t>(&model_CompiledMap_SourceCallback_Float));
        }

        func = module->getFunction(moduleName + "_CompiledMap_SinkCallback_Float");
        if (func != nullptr)
        {
            jitter.DefineFunction(func, reinterpret_cast<uint64_t>(&model_CompiledMap_SinkCallback_Float));
        }
    };

    return Map::Compile(targetDevice, moduleName, functionName, "CompiledMap_SourceCallback_Float", "CompiledMap_SinkCallback_Float", compilerSettings, optimizerSettings, resolverFunction);
}

CompiledMap Map::Compile(const std::string& targetDevice,
                         const std::string& moduleName,
                         const std::string& functionName,
                         const std::string& sourceFunctionName,
                         const std::string& sinkFunctionName,
                         const MapCompilerOptions& compilerSettings,
                         const ModelOptimizerOptions& optimizerSettings,
                         std::function<void(llvm::Module*, ell::emitters::IRExecutionEngine&)> resolveCallbacks) const
{
    ell::model::MapCompilerOptions settings;
    settings.moduleName = moduleName;
    settings.mapFunctionName = functionName;
    settings.sourceFunctionName = sourceFunctionName;
    settings.sinkFunctionName = sinkFunctionName;
    settings.compilerSettings.targetDevice.deviceName = targetDevice;
    settings.compilerSettings.useBlas = compilerSettings.useBlas;
    settings.optimizerSettings.fuseLinearFunctionNodes = optimizerSettings.fuseLinearFunctionNodes;

    ell::model::IRMapCompiler compiler(settings);

    auto module = compiler.GetModule().GetLLVMModule();
    auto compiledMap = compiler.Compile(*_map);
    if (!sourceFunctionName.empty() || !sinkFunctionName.empty())
    {
        resolveCallbacks(module, compiledMap.GetJitter());
    }
    return CompiledMap(std::move(compiledMap), GetInputShape(), GetOutputShape());
}

//
// CompiledMap
//
CompiledMap::CompiledMap(ell::model::IRCompiledMap map, ell::api::math::TensorShape inputShape, ell::api::math::TensorShape outputShape)
    : _inputShape(inputShape), _outputShape(outputShape)
{
    _map = std::make_shared<ell::model::IRCompiledMap>(std::move(map));
}

CompiledMap::~CompiledMap()
{
    UnregisterCallbacks<double>();
    UnregisterCallbacks<float>();
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

bool CompiledMap::HasSourceNodes()
{
    if (_sourceNodeState == TriState::Uninitialized)
    {
        // lazily search for SourceNode and cache the answer so next call is much faster.
        auto sourceNodes = _map->GetModel().GetNodesByType<ell::model::SourceNodeBase>();
        _sourceNodeState = sourceNodes.empty() ? TriState::No : TriState::Yes;
    }
    return _sourceNodeState == TriState::Yes;
}

std::vector<double> CompiledMap::ComputeDouble(const std::vector<double>& inputData)
{
    if (_map != nullptr)
    {
        return _map->Compute<double>(inputData);
    }
    return {};
}

std::vector<float> CompiledMap::ComputeFloat(const std::vector<float>& inputData)
{
    if (_map != nullptr)
    {
        return _map->Compute<float>(inputData);
    }
    return {};
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

// Specializations with type-specific static forwarder instances
template <>
ell::api::CallbackForwarder<double, double>& CompiledMap::GetCallbackForwarder()
{
    return forwarderDouble;
}

// Specializations with type-specific static forwarder instances
template <>
ell::api::CallbackForwarder<float, float>& CompiledMap::GetCallbackForwarder()
{
    return forwarderFloat;
}
}
