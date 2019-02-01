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

#include <common/include/LoadModel.h>
#include <common/include/MapLoadArguments.h>
#include <common/include/RegisterNodeCreators.h>

#include <data/include/DenseDataVector.h>

#include <dsp/include/FilterBank.h>

#include <emitters/include/ModuleEmitter.h>

#include <model/include/InputNode.h>
#include <model/include/Map.h>
#include <model/include/OutputNode.h>

#include <utilities/include/JsonArchiver.h>
#include <utilities/include/StringUtil.h>

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

void model_CompiledMap_LagNotificationCallback(void* context, double lag)
{
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

Port::Port(const ell::model::Port* other) :
    _port(other)
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

PortMemoryLayout Port::GetMemoryLayout()
{
    return PortMemoryLayout(_port->GetMemoryLayout());
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

InputPortIterator::InputPortIterator(const std::vector<const ell::model::InputPortBase*>& ports) :
    _i(0),
    _ports(ports)
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

OutputPortIterator::OutputPortIterator(const std::vector<const ell::model::OutputPortBase*>& ports) :
    _i(0),
    _ports(ports)
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

NodeIterator::NodeIterator(const std::vector<const ell::model::Node*>& nodes) :
    _i(0),
    _isVector(true),
    _nodes(nodes),
    _iterator()
{
}

NodeIterator::NodeIterator(ell::model::ForwardNodeIterator& other) :
    _i(0),
    _isVector(false),
    _nodes(0),
    _iterator(other)
{
}

//
// Node
//
Node::Node(const ell::model::Node* other) :
    _node(other)
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
    auto ports = _node->GetOutputPorts();
    std::vector<const ell::model::OutputPortBase*> constPorts(ports.begin(), ports.end());
    return OutputPortIterator(constPorts);
}

InputPortIterator Node::GetInputPorts()
{
    auto ports = _node->GetInputPorts();
    std::vector<const ell::model::InputPortBase*> constPorts(ports.begin(), ports.end());
    return InputPortIterator(constPorts);
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

void Node::CopyMetadataFrom(const Node& other)
{
    const_cast<ell::model::Node*>(_node)->GetMetadata() = other._node->GetMetadata();
}

//
// InputNode
//
InputNode::InputNode(const InputNode& node) :
    Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::InputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an InputNode");
    }
}

InputNode::InputNode(Node node) :
    Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::InputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an InputNode");
    }
}

InputNode::InputNode(const ell::model::InputNodeBase* other) :
    Node(other)
{
}

const ell::model::InputNodeBase* InputNode::GetInputNode() const
{
    return dynamic_cast<const ell::model::InputNodeBase*>(GetNode());
}

//
// OutputNode
//
OutputNode::OutputNode(const OutputNode& node) :
    Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::OutputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an OutputNode");
    }
}

OutputNode::OutputNode(Node node) :
    Node(node.GetNode())
{
    if (dynamic_cast<const ell::model::OutputNodeBase*>(node.GetNode()) == nullptr)
    {
        throw std::invalid_argument("Error: not an OutputNode");
    }
}

OutputNode::OutputNode(const ell::model::OutputNodeBase* other) :
    Node(other)
{
}

const ell::model::OutputNodeBase* OutputNode::GetOutputNode() const
{
    return dynamic_cast<const ell::model::OutputNodeBase*>(GetNode());
}

//
// PortElement
//
PortElement::PortElement(const ell::model::PortElementBase& other) :
    _port(other)
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
PortElements::PortElements(const ell::model::PortElementsBase& other) :
    _elements(other)
{
}

PortElements::PortElements(const OutputPort& port) :
    _elements(port.GetOutputPort())
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

InputPort::InputPort(const ell::model::InputPortBase* other) :
    Port(other),
    _input_port(other)
{
}

NodeIterator InputPort::GetParentNodes()
{
    return NodeIterator(_input_port->GetParentNodes());
}

OutputPort InputPort::GetReferencedPort()
{
    return OutputPort(&_input_port->GetReferencedPort());
}

//
// OutputPort
//
OutputPort::OutputPort(const ell::model::OutputPortBase* other) :
    Port(other),
    _output_port(other)
{
}

bool OutputPort::IsReferenced() const
{
    return _output_port->IsReferenced();
}

InputPortIterator OutputPort::GetReferences()
{
    return InputPortIterator(_output_port->GetReferences());
}

std::vector<double> OutputPort::GetDoubleOutput()
{
    return _output_port->GetDoubleOutput();
}

double OutputPort::GetDoubleOutput(int index)
{
    return _output_port->GetDoubleOutput((size_t)index);
}

//
// PortMemoryLayout
//

bool IsAllZeros(const std::vector<int>& p)
{
    if (p.size() == 0) 
    {
        return true;
    }
    return std::all_of(p.begin(), p.end(), [](const int& value) { return value == 0; });
}

ell::model::PortMemoryLayout GetPortMemoryLayout(const std::vector<int>& size, const std::vector<int>& padding, const std::vector<int>& offset, const std::vector<int>& order)
{
    if (IsAllZeros(padding) && IsAllZeros(offset))
    {
        return ell::model::PortMemoryLayout(ell::model::MemoryShape{ size });
    }
    else if (IsAllZeros(offset))
    {
        return ell::model::PortMemoryLayout(ell::model::MemoryShape{ size }, ell::model::MemoryShape{ padding });
    }
    else if (IsAllZeros(order))
    {
        return ell::model::PortMemoryLayout(ell::model::MemoryShape{ size }, ell::model::MemoryShape{ padding }, ell::model::MemoryShape{ offset });
    }
    else
    {
        return ell::model::PortMemoryLayout(ell::model::MemoryShape{ size }, ell::model::MemoryShape{ padding }, ell::model::MemoryShape{ offset }, ell::model::DimensionOrder{ order });
    }
}

PortMemoryLayout::PortMemoryLayout(const std::vector<int>& s, const std::vector<int>& p, const std::vector<int>& o, const std::vector<int>& order) :
    PortMemoryLayout(GetPortMemoryLayout(s,p,o,order))
{
}

PortMemoryLayout::PortMemoryLayout(const ell::model::PortMemoryLayout& layout) :
    size(layout.GetActiveSize().ToVector()),
    padding(layout.GetExtent().ToVector()),
    offset(layout.GetOffset().ToVector()),
    order(layout.GetLogicalDimensionOrder().ToVector()),
    _layout(layout)
{
}

bool PortMemoryLayout::IsEqual(const PortMemoryLayout& other)
{
    return _layout == other._layout;
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

std::string GetVariableName(const ell::model::PortElementsBase& portElements, const std::string& defaultName)
{
    // a PortElements object can pull output from multiple nodes, so the friendly name should then be
    // the concatenation of friendly names of each of those output nodes.
    std::vector<std::string> parts;
    for (auto range: portElements.GetRanges())
    {
        const ell::model::OutputPortBase* outputPort = range.ReferencedPort();
        std::string name = outputPort->GetVariableName(defaultName);
        parts.push_back(name);
    }
    if (parts.size() == 0) {
        return defaultName;
    }
    return ell::utilities::Join(parts, "_");
}

Map::Map(Model model, InputNode inputNode, PortElements output)
{
    const ell::model::InputNodeBase* innerInputNode = inputNode.GetInputNode();
    std::string name = innerInputNode->GetFriendlyName();
    if (name.empty())
    {
        name = "input";
    }

    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> inputs = { std::pair<std::string, ell::model::InputNodeBase*>{ name, const_cast<ell::model::InputNodeBase*>(innerInputNode) } };

    const ell::model::PortElementsBase& innerPortElements = output.GetPortElements();
    name = GetVariableName(innerPortElements, "output");
    auto outputs = std::vector<std::pair<std::string, ell::model::PortElementsBase>>{ { name, innerPortElements } };
    _map = std::make_shared<ell::model::Map>(model.GetModel(), inputs, outputs);
}

Map::Map(Model model, const std::vector<InputNode*> inputNodes, const std::vector<PortElements*> outputs)
{
    std::vector<std::pair<std::string, ell::model::InputNodeBase*>> mapInputs;
    for (auto& inputNode: inputNodes)
    {
        const ell::model::InputNodeBase* innerInputNode = inputNode->GetInputNode();
        auto name = innerInputNode->GetFriendlyName();
        if (name.empty())
        {
            name = "input";
        }
        mapInputs.push_back(std::pair<std::string, ell::model::InputNodeBase*>{ name, const_cast<ell::model::InputNodeBase*>(innerInputNode) });
    }
        
    std::vector<std::pair<std::string, ell::model::PortElementsBase>> mapOutputs;
    for (auto& output : outputs)
    {
        const ell::model::PortElementsBase& innerPortElements = output->GetPortElements();
        auto name = GetVariableName(innerPortElements, "output");
        mapOutputs.push_back({ name, innerPortElements });
    }
    _map = std::make_shared<ell::model::Map>(model.GetModel(), mapInputs, mapOutputs);
}

Map::Map(std::shared_ptr<ell::model::Map>& map) :
    _map(map)
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
    return Model(_map->GetModel().ShallowCopy());
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

void ResolveCallbacks(llvm::Module* module, ell::emitters::IRExecutionEngine& jitter)
{
    for (llvm::Function& func : module->getFunctionList())
    {
        std::string name = func.getName().str();
        if (func.hasExternalLinkage())
        {
            if (name.find("LagNotification") != std::string::npos)
            {
                jitter.DefineFunction(&func, reinterpret_cast<uintptr_t>(&model_CompiledMap_LagNotificationCallback));
            }
            else if (name.find("SourceCallback_Float") != std::string::npos)
            {
                jitter.DefineFunction(&func, reinterpret_cast<uintptr_t>(&model_CompiledMap_SourceCallback_Float));
            }
            else if (name.find("SinkCallback_Float") != std::string::npos)
            {
                jitter.DefineFunction(&func, reinterpret_cast<uintptr_t>(&model_CompiledMap_SinkCallback_Float));
            }
            else if (name.find("SourceCallback_Double") != std::string::npos)
            {
                jitter.DefineFunction(&func, reinterpret_cast<uintptr_t>(&model_CompiledMap_SourceCallback_Double));
            }
            else if (name.find("SinkCallback_Double") != std::string::npos)
            {
                jitter.DefineFunction(&func, reinterpret_cast<uintptr_t>(&model_CompiledMap_SinkCallback_Double));
            }
            else
            {
                // the predict function and the openblas_gemm functions also have external linkage.
            }
        }
    }
}

CompiledMap Map::CompileDouble(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName, const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings) const
{
    return Map::Compile(targetDevice, moduleName, functionName, "CompiledMap_SourceCallback_Double", "CompiledMap_SinkCallback_Double", compilerSettings, optimizerSettings, &ResolveCallbacks);
}

CompiledMap Map::CompileFloat(const std::string& targetDevice, const std::string& moduleName, const std::string& functionName, const MapCompilerOptions& compilerSettings, const ModelOptimizerOptions& optimizerSettings) const
{
    return Map::Compile(targetDevice, moduleName, functionName, "CompiledMap_SourceCallback_Float", "CompiledMap_SinkCallback_Float", compilerSettings, optimizerSettings, &ResolveCallbacks);
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
CompiledMap::CompiledMap(ell::model::IRCompiledMap map, ell::api::math::TensorShape inputShape, ell::api::math::TensorShape outputShape) :
    _inputShape(inputShape),
    _outputShape(outputShape)
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
} // namespace ELL_API
