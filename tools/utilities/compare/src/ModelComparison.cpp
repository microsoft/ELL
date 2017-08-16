////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelComparison.cpp
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelComparison.h"
#include "CompareUtils.h"
#include "VectorStats.h"

// emitters
#include "EmitterTypes.h"

// utilities
#include "DgmlGraph.h"
#include "Files.h"

// stl
#include <sstream>
#include <string>

extern "C" {
void DebugOutput(char* label, float* output, void* userData)
{
    ModelComparison* self = static_cast<ModelComparison*>(userData);
    self->AddLayer(label, output);
}
}

//
// Utility functions
//

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
    return {result.begin(), result.end()};
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
            // model::ValueType<model::Port::PortType::bigInt>>
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

void ModelComparison::Compare(std::vector<float>& input, model::DynamicMap& reference, bool useBlas, bool optimize)
{
    SetUpReferenceMap(reference);
    _addingReference = false;

    // Ok, now compile the model with debug set to true so we can get the DebugOutput
    // function calls to compare compiled model with reference implementation.

    model::MapCompilerParameters settings;
    settings.compilerSettings.useBlas = useBlas;
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

    model::IRCompiledMap compiledMap = compiler.Compile(reference);

    // Windows can not do automatic resolution of symbols, so it won't find the DebugOutput function above unless I help it here.
    auto func = module->getFunction("DebugOutput");
    compiledMap.GetJitter().DefineFunction(func, reinterpret_cast<uint64_t>(&DebugOutput));

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
    DgmlGraph::ReplaceAll(fileSafeId, "<", "_");
    DgmlGraph::ReplaceAll(fileSafeId, ">", "_");

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
    for(auto& layerData: _layerOutputData)
    {
        if (layerData.compiledNodeLabel != "")
        {
            std::string id = layerData.compiledNodeId;
            std::string label = layerData.compiledNodeLabel;
            WriteRow(outputStream, id, label, layerData.referenceData, layerData.compiledData, &layerData);
        }
        else
        {
            //std::cout << "Compiled data missing for reference node " << layerData.referenceNodeLabel << std::endl;
        }
    }
}

std::string to_string(std::vector<size_t> shape)
{
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0, length = shape.size(); i < length; i++)
    {
        if (i > 0)
        {
            ss << ", ";
        }
        ss << shape[i];
    }
    ss << " ]";
    return ss.str();
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

    VectorStats refStats(reference);
    VectorStats compiledStats(compiled);
    float diff = VectorStats::Diff(reference, compiled);
    if (layerData != nullptr)
    {
        if (!_hasMinMax)
        {
            _minError = _maxError = diff;
            _hasMinMax = true;
        }
        else
        {
            _minError = fmin(_minError, diff);
            _maxError = fmax(_maxError, diff);
        }
    }
    outputStream << "````" << std::endl;
    if (layerData != nullptr)
    {
        outputStream << "size=" << to_string(layerData->size) << ", stride=" << to_string(layerData->stride) << ", offset=" << to_string(layerData->offset) << std::endl;
    }
    outputStream << "reference: min=" << refStats.Min() << ", max=" << refStats.Max() << ", mean=" << refStats.Mean() << ", stddev=" << refStats.StdDev() << ", var=" << refStats.Variance() << std::endl;
    outputStream << "compiled : min=" << compiledStats.Min() << ", max=" << refStats.Max() << ", mean=" << refStats.Mean() << ", stddev=" << refStats.StdDev() << ", var=" << refStats.Variance() << std::endl;
    outputStream << "difference: " << diff << std::endl;
    outputStream << "````" << std::endl;
    outputStream << std::endl;

    if (id != "")
    {
        id = _nodeMap[id]; // map old id to new id
        DgmlNode* node = _graph.GetNode(id);
        if (node != nullptr)
        {
            node->Properties["Error"] = std::to_string(diff);
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

    DgmlProperty p;
    p.Id = "Error";
    p.Label = "Error";
    p.Description = "Amount of error between compiled and reference layers";
    p.DataType = "double";
    _graph.AddProperty(p);

    DgmlStyle es;
    es.TargetType = "Node";
    es.GroupLabel = "Error";
    es.ValueLabel = "Gradient";
    es.Condition.Expression = "Error > 0";
    DgmlStyleSetter red;
    red.Property = "Background";
    red.Expression = "Color.FromRgb(55 + 200 * (" + expr + " + " + std::to_string(min) + ") / " + std::to_string(range) + ", 0, 0)";
    es.Setters.push_back(red);

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
        for(auto& layerData: _layerOutputData)
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
        for(auto& layerData: _layerOutputData)
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
            DgmlNode childNode = _graph.GetOrCreateNode(to_string(node.GetId()), typeName);
            auto dependencies = node.GetDependentNodes();
            for (auto ptr = dependencies.begin(), end = dependencies.end(); ptr != end; ptr++)
            {
                const model::Node* upstream = *ptr;
                if (upstream != nullptr)
                {
                    // the link points from childNode to nextNode implying a flow of data from childNode to nextNode which
                    // is what we want because nextNode is "dependent on" childNode, meaning it consumes the output from
                    // childNode, so the data is flowing from childNode to the nextNode.
                    if(!IsDebugSinkNode(*upstream))
                    {
                        std::string id = to_string(upstream->GetId());
                        std::string typeName = upstream->GetRuntimeTypeName();
                        DgmlNode& nextNode = _graph.GetOrCreateNode(id, typeName);
                        _graph.GetOrCreateLink(childNode, nextNode, "");
                    }
                }
            }
        }
    });
}

void ModelComparison::SaveGraph(std::ostream& stm)
{
    AddStyles();
    // and add <Styles> section to the graph to that is clearly visible.
    _graph.Save(stm);
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
    // todo: add additional argument to the sink function telling user which layer this is...
    std::string sinkFunctionName = "DebugOutput";
    auto newPortElements = transformer.GetCorrespondingOutputs(layerNode->output);
    std::string label = layerNode->GetRuntimeTypeName() + "(" + to_string(layerNode->GetId()) + ")";

    size_t size = layerNode->GetOutputSize();
    _outputSizes[label] = size;

    auto sinkFunction = [this](const std::string& label, const std::vector<ValueType>& output, void* userData) {
        AddLayer(label.c_str(), &output[0]);
    };

    auto newNode = transformer.AddNode<ell::nodes::DebugSinkNode<ValueType>>(
        newPortElements,
        sinkFunction,
        label,
        static_cast<void*>(this),
        sinkFunctionName);

    if (_addingReference)
    {
        LayerCaptureData layerData;
        layerData.compiledDebugNode = nullptr;
        layerData.referenceDebugNode = newNode;
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
            layerData.compiledDebugNode = newNode;
            auto memLayout = layerNode->GetOutputMemoryLayout();
            layerData.size = memLayout.size;
            layerData.stride = memLayout.stride;
            layerData.offset = memLayout.offset;
            layerData.compiledNodeId = to_string(layerNode->GetId());
            layerData.compiledNodeLabel = label;
            auto referenceLabel = GetSinkNodeLabel(layerData.referenceDebugNode);
            _nodeMap[referenceLabel] = label;
        }
    }
}
