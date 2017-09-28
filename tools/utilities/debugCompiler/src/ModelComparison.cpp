////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelComparison.cpp
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CompareUtils.h"
#include "ModelComparison.h"
#include "VectorStatistics.h"

// emitters
#include "EmitterTypes.h"

// nodes
#include "ReorderDataNode.h"
#include "DebugSinkNode.h"

// utilities
#include "Graph.h"
#include "Files.h"

// stl
#include <sstream>
#include <string>

extern "C" {
void DebugOutput(char* label, float* output, void* userData)
{
    if(userData != nullptr)
    {
        ModelComparison* self = static_cast<ModelComparison*>(userData);
        self->AddLayer(label, output);
    }
}
}

using namespace ell::utilities;

//
// Utility functions
//

template <typename ValueType>
std::ostream& operator<<(std::ostream& os, const std::vector<ValueType>& vec)
{
    os << "[";
    for (size_t i = 0, length = vec.size(); i < length; i++)
    {
        if (i > 0)
        {
            os << ", ";
        }
        os << vec[i];
    }
    os << " ]";
    return os;
}

template <typename ValueType>
bool IsDebugSinkNode(const model::Node& node)
{
    auto sinkNode = dynamic_cast<const nodes::DebugSinkNode<ValueType>*>(&node);
    return sinkNode != nullptr;
}

bool IsDebugSinkNode(const model::Node& node)
{
    return IsDebugSinkNode<bool>(node) || IsDebugSinkNode<int>(node) || IsDebugSinkNode<float>(node) || IsDebugSinkNode<double>(node);
}

template <typename ValueType>
std::string GetDebugSinkNodeLabel(const model::Node& node)
{
    const auto& sinkNode = dynamic_cast<const nodes::DebugSinkNode<ValueType>&>(node);
    return sinkNode.GetLabel();
}

std::string GetDebugSinkNodeLabel(const model::Node& node)
{
    if (IsDebugSinkNode<bool>(node))
    {
        return GetDebugSinkNodeLabel<bool>(node);
    }
    else if (IsDebugSinkNode<int>(node))
    {
        return GetDebugSinkNodeLabel<int>(node);
    }
    else if (IsDebugSinkNode<float>(node))
    {
        return GetDebugSinkNodeLabel<float>(node);
    }
    else if (IsDebugSinkNode<double>(node))
    {
        return GetDebugSinkNodeLabel<double>(node);
    }
    return "";
}

template <typename InputType, typename OutputType>
std::vector<float> GetMapOutput(const model::DynamicMap& map, const std::vector<float>& input)
{
    std::vector<InputType> typedInput(input.begin(), input.end());
    auto result = map.Compute<InputType>(typedInput);
    return { result.begin(), result.end() };
}

template <typename InputType>
std::vector<float> GetMapOutput(const model::DynamicMap& map, const std::vector<float>& input)
{
    switch (map.GetOutputType())
    {
        case model::Port::PortType::smallReal:
            return GetMapOutput<InputType, float>(map, input);
            break;
        case model::Port::PortType::real:
            return GetMapOutput<InputType, double>(map, input);
            break;
        case model::Port::PortType::integer:
            return GetMapOutput<InputType, int>(map, input);
            break;
        case model::Port::PortType::bigInt:
            return GetMapOutput<InputType, int64_t>(map, input);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Model has an unsupported output type");
    }
}

std::vector<float> GetMapOutput(const model::DynamicMap& map, const std::vector<float>& input)
{
    switch (map.GetInputType())
    {
        case model::Port::PortType::smallReal:
            return GetMapOutput<float>(map, input);
            break;
        case model::Port::PortType::real:
            return GetMapOutput<double>(map, input);
            break;
        case model::Port::PortType::integer:
            return GetMapOutput<int>(map, input);
            break;
        case model::Port::PortType::bigInt:
            return GetMapOutput<int64_t>(map, input);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Model has an unsupported input type");
    }
}

//
// ModelComparison implementation
//
ModelComparison::ModelComparison(std::string outputDirectory)
{
    _outputDirectory = outputDirectory;
    _runningCompiled = false;
    _addingReference = false;
    _minError = 0;
    _maxError = 0;
    _hasMinMax = false;
}

void ModelComparison::SetUpReferenceMap(model::DynamicMap& map)
{
    _addingReference = true;

    // keep a copy of the uncompiled map so we can run the reference implementation later.
    _referenceMap = map;

    // Refine to get the layer nodes so we can inject the DebugSinkNodes between each layer of the neural net.
    // this causes AddDebugOutputNode to be called where we save the some information about each DebugSinkNodes created.
    auto refineLayerFunc = [](const model::Node& node) {
        return IsNeuralNetworkPredictorNode(&node) ? model::NodeAction::refine : model::NodeAction::compile;
    };
    model::TransformContext refineContext(refineLayerFunc);
    _referenceMap.Refine(refineContext);

    // Now add the debug sink nodes
    model::TransformContext addSinkNodeContext;
    auto transformFunc = [this](const ell::model::Node& node, ell::model::ModelTransformer& transformer) {
        node.Copy(transformer);

        if (IsNeuralNetworkLayerNode(&node))
        {
            AddDebugOutputNode(transformer, node);
        }
    };
    _referenceMap.Transform(transformFunc, addSinkNodeContext);
}

void ModelComparison::Compare(std::vector<float>& input, model::DynamicMap& reference, bool useBlas, bool optimize, bool allowVectorInstructions)
{
    SetUpReferenceMap(reference);
    _addingReference = false;

    // Ok, now compile the model with debug set to true so we can get the DebugOutput
    // function calls to compare compiled model with reference implementation.

    model::MapCompilerParameters settings;
    settings.compilerSettings.useBlas = useBlas;
    settings.compilerSettings.allowVectorInstructions = allowVectorInstructions;
    settings.compilerSettings.optimize = optimize;
    settings.profile = false;
    settings.compilerSettings.targetDevice.deviceName = "host";

    // now repeat the refine step again, this time when AddDebugOutputNode is called we save the compiled Node information.
    auto refineLayerFunc = [](const model::Node& node) {
        return IsNeuralNetworkPredictorNode(&node) ? model::NodeAction::refine : model::NodeAction::compile;
    };
    model::TransformContext refineContext(refineLayerFunc);
    reference.Refine(refineContext);

    // Now add the debug sink nodes
    model::TransformContext context;
    auto transformFunc = [this](const ell::model::Node& node, ell::model::ModelTransformer& transformer) {
        node.Copy(transformer);

        if (IsNeuralNetworkLayerNode(&node))
        {
            AddDebugOutputNode(transformer, node);
        }
    };

    reference.Transform(transformFunc, context);

    model::IRMapCompiler compiler(settings);

    // Grab a pointer to the module before TransferOwnership nulls it out.
    llvm::Module* module = compiler.GetModule().GetLLVMModule();
    std::cout << "compiling..." << std::endl;

    void* temp = (void*)(&DebugOutput);
    model::IRCompiledMap compiledMap = compiler.Compile(reference);

    // Windows can not do automatic resolution of symbols, so it won't find the DebugOutput function above unless I help it here.
    auto func = module->getFunction("DebugOutput");
    if(func != nullptr)
    {
        compiledMap.GetJitter().DefineFunction(func, reinterpret_cast<uint64_t>(&DebugOutput));
    }
    else
    {
        std::cout << "Got null DebugOutput function" << std::endl;
    }

    std::cout << "jitting..." << std::endl;
    compiledMap.FinishJitting();

    std::cout << "executing..." << std::endl;

    // build the graph
    CreateGraph(compiledMap.GetModel());

    // Compute reference output
    _runningCompiled = false;
    _outputReference = GetMapOutput(_referenceMap, input);

    // Gather the reference model DebugNodeSinks
    std::vector<const ell::model::Node*> referenceNodes;
    for (size_t i = 0, length = _layerOutputData.size(); i < length; i++)
    {
        LayerCaptureData& layerData = _layerOutputData[i];
        if (layerData.referenceDebugNode != nullptr)
        {
            referenceNodes.push_back(layerData.referenceDebugNode);
        }
    }

    // Now the normal reference.Compute will skip my DebugSinkNodes, so now I have to do another VisitSubset to gather that output.
    // This will cause the DebugSinkNode Sink function to fire which will call AddLayer below.
    auto compute = [](const model::Node& node) { node.Compute(); };
    _referenceMap.GetModel().VisitSubset(referenceNodes, compute);

    // Compute compiled output
    _runningCompiled = true;
    _outputCompiled = GetMapOutput(compiledMap, input);
}

void ModelComparison::SaveOutput(std::string name, const std::vector<float>& reference, const std::vector<float>& compiled)
{
    std::string fileSafeId = name;
    ReplaceAll(fileSafeId, "<", "_");
    ReplaceAll(fileSafeId, ">", "_");

    auto fullPath = utilities::JoinPaths(_outputDirectory, fileSafeId + ".csv");
    std::ofstream data(fullPath);
    data << "reference,compiled\n";

    for (size_t i = 0, len1 = reference.size(), len2 = compiled.size(); i < len1 && i < len2; i++)
    {
        data << reference[i] << "," << compiled[i] << "\n";
    }
}

size_t ModelComparison::GetOutputSize(const std::string& nodeId)
{
    return _outputSizes[nodeId];
}

void ModelComparison::WriteReport(std::ostream& outputStream, std::string modelName, std::string testDataName)
{
    std::cout << "writing report..." << std::endl;

    outputStream << "# Comparison Results" << std::endl;
    outputStream << "**model**: " << modelName << std::endl;
    outputStream << std::endl;
    outputStream << "**image**: " << testDataName << std::endl;
    outputStream << std::endl;

    WriteRow(outputStream, "", "Overall", _outputReference, _outputCompiled, nullptr);
    for (auto& layerData : _layerOutputData)
    {
        if (layerData.compiledNodeLabel != "")
        {
            std::string id = layerData.compiledNodeId;
            std::string label = layerData.compiledNodeLabel;
            WriteRow(outputStream, id, label, layerData.referenceData, layerData.compiledData, &layerData);
        }
        else
        {
            std::cout << "Compiled data missing for reference node " << layerData.referenceNodeLabel << std::endl;
        }
    }
}

void ModelComparison::WriteRow(std::ostream& outputStream, std::string id, std::string name, const std::vector<float>& reference, const std::vector<float>& compiled, LayerCaptureData* layerData)
{
    if (compiled.size() == 0)
    {
        // Layer was pruned from compiled model
        return;
    }

    SaveOutput("Compare_" + name, reference, compiled);

    outputStream << "## " << name << std::endl;

    VectorStatistics refStats(reference);
    VectorStatistics compiledStats(compiled);
    VectorStatistics diffStats(Abs(Subtract(reference, compiled)));
    
    float absDiffSum = VectorStatistics::Diff(reference, compiled);
    if (layerData != nullptr)
    {
        if (!_hasMinMax)
        {
            _minError = _maxError = absDiffSum;
            _hasMinMax = true;
        }
        else
        {
            _minError = fmin(_minError, absDiffSum);
            _maxError = fmax(_maxError, absDiffSum);
        }
    }
    outputStream << "````" << std::endl;
    if (layerData != nullptr)
    {
        const ell::model::Node* node = layerData->referenceDebugNode;
        if (node != nullptr)
        {
            WriteNodeDetail(outputStream, node);
        }
        else
        {
            outputStream << "output size = " << layerData->size << ", datasize = " << layerData->stride << ", offset = " << layerData->offset << std::endl;
        }
    }

    // TODO: write out as a table?
    // |           |      min     |      max |  mean |    stddev |         var |
    // |-----------|--------------|----------|-------|-----------|-------------|
    // | reference |  5.30016e-19 | 0.943143 | 0.001 | 0.0298175 | 0.000889082 |
    // | compiled  |  5.30014e-19 | 0.943143 | 0.001 | 0.0298175 | 0.000889082 |
    
    outputStream << "reference : min = " << refStats.Min() << ", max = " << refStats.Max() << ", mean = " << refStats.Mean() << ", stddev = " << refStats.StdDev() << ", var = " << refStats.Variance() << std::endl;
    outputStream << "compiled  : min = " << compiledStats.Min() << ", max = " << compiledStats.Max() << ", mean = " << compiledStats.Mean() << ", stddev = " << compiledStats.StdDev() << ", var = " << compiledStats.Variance() << std::endl;
    outputStream << "difference: min = " << diffStats.Min() << ", max = " << diffStats.Max() << ", mean = " << diffStats.Mean() << ", stddev = " << diffStats.StdDev() << ", var = " << diffStats.Variance() << std::endl;
    outputStream << "sum of absolute differences: " << absDiffSum << std::endl;
    outputStream << "N: " << refStats.NumElements() << std::endl;
    outputStream << "````" << std::endl;
    outputStream << std::endl;

    if (id != "")
    {
        id = _nodeMap[id]; // map old id to new id
        GraphNode* node = _graph.GetNode(id);
        if (node != nullptr)
        {
            node->GetProperties()["Error"] = std::to_string(absDiffSum);
        }
    }
}

template <typename ValueType>
void ModelComparison::WriteLayerNodeDetail(std::ostream& outputStream, const ell::nodes::NeuralNetworkLayerNodeBase<ValueType>* layerNode)
{
    auto layerType = layerNode->GetRuntimeTypeName();
    auto inputLayout = layerNode->GetInputMemoryLayout();
    auto inputLayoutSize = inputLayout.GetActiveSize();
    auto inputLayoutStride = inputLayout.GetStride();
    auto inputLayoutOffset = inputLayout.GetOffset();

    auto outputLayout = layerNode->GetOutputMemoryLayout();
    auto outputLayoutSize = outputLayout.GetActiveSize();
    auto outputLayoutStride = outputLayout.GetStride();
    auto outputLayoutOffset = outputLayout.GetOffset();

    outputStream << "Input size:  " << inputLayoutSize[0] << " x " << inputLayoutSize[1] << " x " << inputLayoutSize[2] << ", ";
    outputStream << "stride: " << inputLayoutStride[0] << " x " << inputLayoutStride[1] << " x " << inputLayoutStride[2] << ", ";
    outputStream << "offset: " << inputLayoutOffset[0] << ",  " << inputLayoutOffset[1] << ",  " << inputLayoutOffset[2] << "\n";

    outputStream << "Output size: " << outputLayoutSize[0] << " x " << outputLayoutSize[1] << " x " << outputLayoutSize[2] << ", ";
    outputStream << "stride: " << outputLayoutStride[0] << " x " << outputLayoutStride[1] << " x " << outputLayoutStride[2] << ", ";
    outputStream << "offset: " << outputLayoutOffset[0] << " x " << outputLayoutOffset[1] << " x " << outputLayoutOffset[2] << "\n";
}

void ModelComparison::WriteNodeDetail(std::ostream& outputStream, const model::Node* node)
{
    auto parents = node->GetParentNodes();
    // node should be a DebugSinkNode, which should only have 1 parent
    if (parents.size() != 1)
    {
        outputStream << "Input port sizes: ";
        const auto& ports = node->GetInputPorts();
        for (const auto& port : ports)
        {
            outputStream << port->Size() << "\t";
        }
        outputStream << "\n";
        return;
    }
    else
    {
        parents = parents[0]->GetParentNodes();
        if(parents.size() == 1)
        {
            node = parents[0];
        }
    }

    // Write out more detail if this node is a NN layer node
    const ell::nodes::NeuralNetworkLayerNodeBase<float>* floatLayerNode = dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<float>*>(node);
    if (floatLayerNode != nullptr)
    {
        WriteLayerNodeDetail(outputStream, floatLayerNode);
    }
    else
    {
        const ell::nodes::NeuralNetworkLayerNodeBase<double>* doubleLayerNode = dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<double>*>(node);
        if (doubleLayerNode != nullptr)
        {
            WriteLayerNodeDetail(outputStream, doubleLayerNode);
        }
    }
}

void ModelComparison::AddStyles()
{
    /*
    // show a red gradient based on how much error was found.
    <Style TargetType='Node' GroupLabel='Error' ValueLabel='Gradient'>
        <Condition Expression='Error &gt; 0'/>
        <Setter Property='Background' Expression='Color.FromRgb(55 + 200 * ( Error + min) / range, 0, 0)'/>
    </Style>
    */
    double min = _minError;
    double max = _maxError;
    double range = max - min;
    std::string expr = "Error";
    if (range > 2550)
    {
        // take the log of the range
        min = log(1 + min);
        max = log(1 + max);
        range = max - min;
        expr = "Math.Log(Error)";
    }

    _graph.AddProperty(GraphProperty{ "Error", "Error", "Amount of error between compiled and reference layers", "double" });

    GraphStyleCondition condition{ "Error > 0" };
    GraphStyle es{ "Node", "Error", "Gradient", condition };
    GraphStyleSetter red{ "Background", "",  "Color.FromRgb(55 + 200 * (" + expr + " + " + std::to_string(min) + ") / " + std::to_string(range) + ", 0, 0)" };
    es.GetSetters().push_back(red);

    // add nice styles to show the errors.
    _graph.AddStyle(es);
}

template <typename ValueType>
void ModelComparison::AddLayer(const char* label, const ValueType* output)
{
    std::string id(label);
    size_t size = GetOutputSize(id);
    std::vector<float> data(output, output + size);
    if (_runningCompiled)
    {
        bool found = false;
        for (auto& layerData : _layerOutputData)
        {
            std::string compiledId = _nodeMap[layerData.referenceNodeLabel];
            if (compiledId == id)
            {
                found = true;
                layerData.compiledData = { data.begin(), data.end() };
                break;
            }
        }
        if (!found)
        {
            std::cout << "### Error: could not find LayerCaptureData for compiled layer " << id << std::endl;
        }
    }
    else
    {
        bool found = false;
        for (auto& layerData : _layerOutputData)
        {
            if (layerData.referenceNodeLabel == id)
            {
                layerData.referenceData = { data.begin(), data.end() };
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::cout << "### Error: could not find LayerCaptureData for reference layer " << id << std::endl;
        }
    }
}

void ModelComparison::CreateGraph(const model::Model& model)
{
    // Create DGML graph of model
    model.Visit([&](const model::Node& node) {
        std::string typeName = node.GetRuntimeTypeName();
        if (IsDebugSinkNode(node))
        {
            // During the process of compilation the DynamicMap Model is cloned a few times which
            // causes the node id's to change, so this creates a map from the original id to the new id
            auto oldLabel = GetDebugSinkNodeLabel(node);

            int start = oldLabel.find("(") + 1;
            int end = oldLabel.find(")");
            auto oldId = oldLabel.substr(start, end - start);
            for (auto parentNode : node.GetInputPort(0)->GetParentNodes())
            {
                std::string newId = to_string(parentNode->GetId());
                std::string newType = parentNode->GetRuntimeTypeName();
                _nodeMap[oldId] = newId;
            }
        }
        else
        {
            GraphNode& childNode = _graph.GetOrCreateNode(to_string(node.GetId()), typeName);
            auto dependencies = node.GetDependentNodes();
            for (auto ptr = dependencies.begin(), end = dependencies.end(); ptr != end; ptr++)
            {
                const model::Node* upstream = *ptr;
                if (upstream != nullptr)
                {
                    // the link points from childNode to nextNode implying a flow of data from childNode to nextNode which
                    // is what we want because nextNode is "dependent on" childNode, meaning it consumes the output from
                    // childNode, so the data is flowing from childNode to the nextNode.
                    if (!IsDebugSinkNode(*upstream))
                    {
                        std::string id = to_string(upstream->GetId());
                        std::string typeName = upstream->GetRuntimeTypeName();
                        GraphNode& nextNode = _graph.GetOrCreateNode(id, typeName);
                        _graph.GetOrCreateLink(childNode, nextNode, "");
                    }
                }
            }
        }
    });
}

void ModelComparison::SaveDgml(std::ostream& stm)
{
    AddStyles();
    // and add <Styles> section to the graph to that is clearly visible.
    _graph.SaveDgml(stm);
}

void ModelComparison::SaveDot(std::ostream& stm)
{
    // and add <Styles> section to the graph to that is clearly visible.
    _graph.SaveDot(stm);
}

void ModelComparison::AddDebugOutputNode(model::ModelTransformer& transformer, const model::Node& node)
{
    auto floatLayerNode = dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<float>*>(&node);
    if (floatLayerNode != nullptr)
    {
        AddDebugOutputNode(transformer, floatLayerNode);
        return;
    }

    auto doubleLayerNode = dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<double>*>(&node);
    if (doubleLayerNode != nullptr)
    {
        AddDebugOutputNode(transformer, doubleLayerNode);
    }
}

template <typename ValueType>
void ModelComparison::AddDebugOutputNode(model::ModelTransformer& transformer, const nodes::NeuralNetworkLayerNodeBase<ValueType>* layerNode)
{
    std::string sinkFunctionName = "DebugOutput";
    auto newPortElements = transformer.GetCorrespondingOutputs(layerNode->output);
    std::string label = layerNode->GetRuntimeTypeName() + "(" + to_string(layerNode->GetId()) + ")";
    
    auto layerOutputLayout = layerNode->GetOutputMemoryLayout();
    auto layerOutputLayoutMinusPadding = ell::model::PortMemoryLayout(layerOutputLayout.GetActiveSize());

    // size_t size = layerNode->GetOutputSize();
    size_t size = layerOutputLayoutMinusPadding.GetMemorySize();
    _outputSizes[label] = size;

    auto reorderNode = transformer.AddNode<ell::nodes::ReorderDataNode<ValueType>>(
        newPortElements,
        layerOutputLayout,
        layerOutputLayout);
        // layerOutputLayoutMinusPadding);
    
    auto nullSinkFunction = [this](const std::string& label, const std::vector<ValueType>& output, void* userData) {
    };

    auto sinkFunction = [this](const std::string& label, const std::vector<ValueType>& output, void* userData) {
        AddLayer(label.c_str(), output.data());
    };
    
    auto nullSinkNode = transformer.AddNode<ell::nodes::DebugSinkNode<ValueType>>(
        newPortElements,
        nullSinkFunction,
        label,
        static_cast<void*>(this),
        sinkFunctionName);

    auto sinkNode = transformer.AddNode<ell::nodes::DebugSinkNode<ValueType>>(
        reorderNode->output,
        sinkFunction,
        label,
        static_cast<void*>(this),
        sinkFunctionName);

    if (_addingReference)
    {
        LayerCaptureData layerData;
        layerData.compiledDebugNode = nullptr;
        layerData.referenceDebugNode = sinkNode;
        layerData.referenceNodeLabel = label;
        _layerOutputData.emplace_back(std::move(layerData));
        _nextIndex = 0;
    }
    else // adding compiled
    {
        int i = _nextIndex++;
        if (i < _layerOutputData.size())
        {
            LayerCaptureData& layerData = _layerOutputData[i];
            layerData.compiledDebugNode = sinkNode;
            auto memLayout = layerNode->GetOutputMemoryLayout();
            layerData.size = memLayout.GetActiveSize();
            layerData.stride = memLayout.GetStride();
            layerData.offset = memLayout.GetOffset();
            layerData.compiledNodeId = to_string(layerNode->GetId());
            layerData.compiledNodeLabel = label;
            auto referenceLabel = GetSinkNodeLabel(layerData.referenceDebugNode);
            _nodeMap[referenceLabel] = label;
        }
    }
}
