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

// common
#include "LoadModel.h"
#include "RegisterNodeCreators.h"

// emitters
#include "ModuleEmitter.h"

// utilities
#include "JsonArchiver.h"

// math
#include "FilterBank.h"
#include "DenseDataVector.h"

// model
#include "InputNode.h"
#include "Map.h"
#include "MapLoadArguments.h"
#include "OutputNode.h"

// nodes
#include "BinaryOperationNode.h"
#include "BufferNode.h"
#include "ClockNode.h"
#include "DCTNode.h"
#include "FFTNode.h"
#include "FilterBankNode.h"
#include "HammingWindowNode.h"
#include "IIRFilterNode.h"
#include "InputNodeBase.h"
#include "NeuralNetworkPredictorNode.h"
#include "Tensor.h"
#include "UnaryOperationNode.h"

//
// Callback functions
//

#ifdef __cplusplus
extern "C" {
#endif

// Note: this currently assumes that there is just 1 source and 1 sink in the map
// Future: extend this to route to multiple sources + sinks based on extra context parameter.
bool model_CompiledMap_SourceCallback_Double(double* input)
{
    return ELL_API::CompiledMap::InvokeSourceCallback(input);
}

bool model_CompiledMap_SourceCallback_Float(float* input)
{
    return ELL_API::CompiledMap::InvokeSourceCallback(input);
}

void model_CompiledMap_SinkCallback_Double(double* output)
{
    ELL_API::CompiledMap::InvokeSinkCallback(output);
}

void model_CompiledMap_SinkCallback_Float(float* output)
{
    ELL_API::CompiledMap::InvokeSinkCallback(output);
}

#ifdef __cplusplus
} // extern "C"
#endif

namespace ELL_API
{

namespace
{
    template <typename OutputType, typename InputType>
    std::vector<OutputType> CastVector(const std::vector<InputType>& vector)
    {
        return { vector.begin(), vector.end() };
    }
}

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
        throw ell::utilities::Exception("no referenced port");
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

int InputPort::Size()
{
    return (int)_port->Size();
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

int OutputPort::Size()
{
    return (int)_port->Size();
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
    ell::utilities::SerializationContext context;
    ell::utilities::JsonUnarchiver ar(stream, context);
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
    ell::utilities::JsonArchiver ar(stream);
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
// ModelBuilder
//
ModelBuilder::ModelBuilder()
{
    ell::common::RegisterNodeCreators(_modelBuilder);
}

Node ModelBuilder::AddNode(Model model, const std::string& nodeType, const std::vector<std::string>& args)
{
    auto newNode = _modelBuilder.AddNode(model.GetModel(), nodeType, args);
    return Node(newNode);
}

Node ModelBuilder::AddDoubleNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<double> predictor)
{
    return AddNeuralNetworkPredictorNode<double>(model, input, predictor);
}

Node ModelBuilder::AddFloatNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<float> predictor)
{
    return AddNeuralNetworkPredictorNode<float>(model, input, predictor);
}

template <typename ElementType>
Node ModelBuilder::AddNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor<ElementType> predictor)
{
    auto elements = ell::model::PortElements<ElementType>(input.GetPortElements());
    auto newNode = model.GetModel().AddNode<ell::nodes::NeuralNetworkPredictorNode<ElementType>>(elements, predictor.GetPredictor());
    return Node(newNode);
}

InputNode ModelBuilder::AddInputNode(Model model, const ell::api::math::TensorShape& tensorShape, PortType type)
{
    using namespace std::string_literals;
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::boolean:
        newNode = model.GetModel().AddNode<ell::model::InputNode<bool>>(tensorShape.ToMathTensorShape());
        break;
    case PortType::integer:
        newNode = model.GetModel().AddNode<ell::model::InputNode<int>>(tensorShape.ToMathTensorShape());
        break;
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::model::InputNode<double>>(tensorShape.ToMathTensorShape());
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::model::InputNode<float>>(tensorShape.ToMathTensorShape());
        break;
    default:
        throw std::invalid_argument("Error: could not create InputNode of the requested type");
    }
    return InputNode(newNode);
}

OutputNode ModelBuilder::AddOutputNode(Model model, const ell::api::math::TensorShape& tensorShape, PortElements input)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::boolean:
        newNode = model.GetModel().AddNode<ell::model::OutputNode<bool>>(ell::model::PortElements<bool>(elements), tensorShape.ToMathTensorShape());
        break;
    case PortType::integer:
        newNode = model.GetModel().AddNode<ell::model::OutputNode<int>>(ell::model::PortElements<int>(elements), tensorShape.ToMathTensorShape());
        break;
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::model::OutputNode<double>>(ell::model::PortElements<double>(elements), tensorShape.ToMathTensorShape());
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::model::OutputNode<float>>(ell::model::PortElements<float>(elements), tensorShape.ToMathTensorShape());
        break;
    default:
        throw std::invalid_argument("Error: could not create OutputNode of the requested type");
    }
    return OutputNode(newNode);
}

Node ModelBuilder::AddClockNode(Model model, PortElements input, double interval, double lagThreshold, const std::string& lagNotificationName)
{
    auto elements = input.GetPortElements();
    auto newNode = model.GetModel().AddNode<ell::nodes::ClockNode>(
        ell::model::PortElements<ell::nodes::TimeTickType>(elements),
        ell::nodes::TimeTickType(interval),
        ell::nodes::TimeTickType(lagThreshold),
        lagNotificationName);
    return Node(newNode);
}

Node ModelBuilder::AddSinkNode(Model model, PortElements input, PortElements trigger, const ell::api::math::TensorShape& tensorShape, const std::string& sinkFunctionName)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    auto triggerElements = trigger.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::SinkNode<double>>(
            ell::model::PortElements<double>(elements),
            ell::model::PortElements<bool>(triggerElements),
            tensorShape.ToMathTensorShape(),
            sinkFunctionName);
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::SinkNode<float>>(
            ell::model::PortElements<float>(elements),
            ell::model::PortElements<bool>(triggerElements),
            tensorShape.ToMathTensorShape(),
            sinkFunctionName);
        break;
    default:
        throw std::invalid_argument("Error: could not create SinkNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddSourceNode(Model model, PortElements input, PortType outputType,
                                 const ell::api::math::TensorShape& tensorShape, const std::string& sourceFunctionName)
{
    auto inputType = input.GetType();
    if (inputType != PortType::real)
    {
        throw std::invalid_argument("Only PortType::real is supported for time signal input");
    }

    using TimeTickType = double;
    auto inputElements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (outputType)
    {
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::SourceNode<double>>(
            ell::model::PortElements<TimeTickType>(inputElements), tensorShape.ToMathTensorShape(), sourceFunctionName);
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::SourceNode<float>>(
            ell::model::PortElements<TimeTickType>(inputElements), tensorShape.ToMathTensorShape(), sourceFunctionName);
        break;
    default:
        throw std::invalid_argument("Error: could not create SourceNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddConstantNode(Model model, std::vector<double> values, PortType type)
{
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::boolean:
        newNode = model.GetModel().AddNode<ell::nodes::ConstantNode<bool>>(CastVector<bool>(values));
        break;
    case PortType::integer:
        newNode = model.GetModel().AddNode<ell::nodes::ConstantNode<int>>(CastVector<int>(values));
        break;
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::ConstantNode<double>>(CastVector<double>(values));
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::ConstantNode<float>>(CastVector<float>(values));
        break;
    default:
        throw std::invalid_argument("Error: could not create ConstantNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddUnaryOperationNode(Model model, PortElements input, UnaryOperationType op)
{
    auto operation = static_cast<ell::emitters::UnaryOperationType>(op);

    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::boolean:
        newNode = model.GetModel().AddNode<ell::nodes::UnaryOperationNode<bool>>(ell::model::PortElements<bool>(elements), operation);
        break;
    case PortType::integer:
        newNode = model.GetModel().AddNode<ell::nodes::UnaryOperationNode<int>>(ell::model::PortElements<int>(elements), operation);
        break;
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::UnaryOperationNode<double>>(ell::model::PortElements<double>(elements), operation);
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::UnaryOperationNode<float>>(ell::model::PortElements<float>(elements), operation);
        break;
    default:
        throw std::invalid_argument("Error: could not create UnaryOperationNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddBinaryOperationNode(Model model, PortElements input1, PortElements input2, BinaryOperationType op)
{
    auto operation = static_cast<ell::emitters::BinaryOperationType>(op);

    auto type = input1.GetType();
    if (type != input2.GetType())
    {
        throw std::invalid_argument("Error: BinaryOperationNode requires both arguments to be of the same type");
    }
    auto elements1 = input1.GetPortElements();
    auto elements2 = input2.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::boolean:
        newNode = model.GetModel().AddNode<ell::nodes::BinaryOperationNode<bool>>(ell::model::PortElements<bool>(elements1), ell::model::PortElements<bool>(elements2), operation);
        break;
    case PortType::integer:
        newNode = model.GetModel().AddNode<ell::nodes::BinaryOperationNode<int>>(ell::model::PortElements<int>(elements1), ell::model::PortElements<int>(elements2), operation);
        break;
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::BinaryOperationNode<double>>(ell::model::PortElements<double>(elements1), ell::model::PortElements<double>(elements2), operation);
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::BinaryOperationNode<float>>(ell::model::PortElements<float>(elements1), ell::model::PortElements<float>(elements2), operation);
        break;
    default:
        throw std::invalid_argument("Error: could not create BinaryOperationNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddIIRFilterNode(Model model, PortElements input, std::vector<double> bCoeffs, std::vector<double> aCoeffs)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::IIRFilterNode<double>>(ell::model::PortElements<double>(elements), bCoeffs, aCoeffs);
        break;
    case PortType::smallReal:
    {
        std::vector<float> bFloatCoeffs(bCoeffs.begin(), bCoeffs.end());
        std::vector<float> aFloatCoeffs(aCoeffs.begin(), aCoeffs.end());
        newNode = model.GetModel().AddNode<ell::nodes::IIRFilterNode<float>>(ell::model::PortElements<float>(elements), bFloatCoeffs, aFloatCoeffs);
    }
    break;
    default:
        throw std::invalid_argument("Error: could not create IIRFilterNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddBufferNode(Model model, PortElements input, int windowSize)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::boolean:
        newNode = model.GetModel().AddNode<ell::nodes::BufferNode<bool>>(ell::model::PortElements<bool>(elements), windowSize);
        break;
    case PortType::integer:
        newNode = model.GetModel().AddNode<ell::nodes::BufferNode<int>>(ell::model::PortElements<int>(elements), windowSize);
        break;
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::BufferNode<double>>(ell::model::PortElements<double>(elements), windowSize);
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::BufferNode<float>>(ell::model::PortElements<float>(elements), windowSize);
        break;
    default:
        throw std::invalid_argument("Error: could not create BufferNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddHammingWindowNode(Model model, PortElements input)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::HammingWindowNode<double>>(ell::model::PortElements<double>(elements));
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::HammingWindowNode<float>>(ell::model::PortElements<float>(elements));
        break;
    default:
        throw std::invalid_argument("Error: could not create HammingWindowNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddFFTNode(Model model, PortElements input)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::FFTNode<double>>(ell::model::PortElements<double>(elements));
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::FFTNode<float>>(ell::model::PortElements<float>(elements));
        break;
    default:
        throw std::invalid_argument("Error: could not create FFTNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddLinearFilterBankNode(Model model, PortElements input, double sampleRate, int numFilters, int numFiltersToUse)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    auto windowSize = elements.Size();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::LinearFilterBankNode<double>>(ell::model::PortElements<double>(elements), ell::dsp::LinearFilterBank(windowSize, sampleRate, numFilters, numFiltersToUse));
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::LinearFilterBankNode<float>>(ell::model::PortElements<float>(elements), ell::dsp::LinearFilterBank(windowSize, sampleRate, numFilters, numFiltersToUse));
        break;
    default:
        throw std::invalid_argument("Error: could not create LinearFilterBankNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddMelFilterBankNode(Model model, PortElements input, double sampleRate, int numFilters, int numFiltersToUse)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    auto windowSize = elements.Size();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::MelFilterBankNode<double>>(ell::model::PortElements<double>(elements), ell::dsp::MelFilterBank(windowSize, sampleRate, numFilters, numFiltersToUse));
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::MelFilterBankNode<float>>(ell::model::PortElements<float>(elements), ell::dsp::MelFilterBank(windowSize, sampleRate, numFilters, numFiltersToUse));
        break;
    default:
        throw std::invalid_argument("Error: could not create MelFilterBankNode of the requested type");
    }
    return Node(newNode);
}

Node ModelBuilder::AddDCTNode(Model model, PortElements input, int numFilters)
{
    auto type = input.GetType();
    auto elements = input.GetPortElements();
    ell::model::Node* newNode = nullptr;
    switch (type)
    {
    case PortType::real:
        newNode = model.GetModel().AddNode<ell::nodes::DCTNode<double>>(ell::model::PortElements<double>(elements), numFilters);
        break;
    case PortType::smallReal:
        newNode = model.GetModel().AddNode<ell::nodes::DCTNode<float>>(ell::model::PortElements<float>(elements), numFilters);
        break;
    default:
        throw std::invalid_argument("Error: could not create DCTNode of the requested type");
    }
    return Node(newNode);
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
    return ell::api::math::TensorShape::FromMathTensorShape(_map->GetInputShape());
}

ell::api::math::TensorShape Map::GetOutputShape() const
{
    return ell::api::math::TensorShape::FromMathTensorShape(_map->GetOutputShape());
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
    _hasSourceNodes = 0;
}

void Map::Save(const std::string& filename) const
{
    ell::common::SaveMap(*_map, filename);
}

bool Map::HasSourceNodes()
{
    // the valid values for _hasSourceNodes are:
    // 0 - means it is unitialized
    // 1 - means the model does not have any source nodes.
    // 2 - means the model has source nodes.
    if (_hasSourceNodes == 0) 
    {
        // lazily search for SourceNode and cache the answer so next call is much faster.
        auto sourceNodes = _map->GetModel().GetNodesByType<ell::model::SourceNodeBase>();
        _hasSourceNodes = sourceNodes.empty() ? 1 : 2;
    }
    return _hasSourceNodes == 2;
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
ell::api::CallbackForwarder<double, double>& CompiledMap::CallbackForwarder()
{
    static ell::api::CallbackForwarder<double, double> forwarderDouble;
    return forwarderDouble;
}

// Specializations with type-specific static forwarder instances
template <>
ell::api::CallbackForwarder<float, float>& CompiledMap::CallbackForwarder()
{
    static ell::api::CallbackForwarder<float, float> forwarderFloat;
    return forwarderFloat;
}
}
