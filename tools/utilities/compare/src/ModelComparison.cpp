
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelComparison.cpp
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ModelComparison.h"
#include "DgmlGraph.h"
#include "VectorStats.h"
#include <sstream>
#include <string>

void DebugTransformer(const ell::model::Node& node, ell::model::ModelTransformer& transformer);
ModelComparison* ModelComparison::_instance = nullptr;

extern "C" {
void DebugOutput(char* label, float* output)
{
    ModelComparison::GetInstance()->AddLayer(label, output);
}
}

ModelComparison::ModelComparison(std::string outputDirectory)
{
    _outputDirectory = outputDirectory;
    _instance = this;
    _runningCompiled = false;
    _addingReference = false;
    _minError = 0;
    _maxError = 0;
    _hasMinMax = false;
}

void ModelComparison::SetupReferenceMap(model::DynamicMap& map)
{
    _addingReference = true;
    // keep a copy of the uncompiled map so we can run the reference implementation later.
    _referenceMap = map;
    _referenceMap.Prune();

    //std::cout << "========== refining reference map ============================" << std::endl;
    // now do the same thing that compiler.Compile does when debug = true.
    _referenceMap.Refine(1);
    model::TransformContext context{ [](const model::Node& node) { return node.IsCompilable() ? model::NodeAction::compile : model::NodeAction::refine; } };
    _referenceMap.Transform(DebugTransformer, context);
}

void ModelComparison::Compare(std::vector<float>& input, model::DynamicMap& reference)
{
    SetupReferenceMap(reference);
    _addingReference = false;

    // Ok, now compile the model with debug set to true so we can get the DebugOutput
    // function calls to compare compiled model with reference implementation.

    model::MapCompilerParameters settings;
    settings.moduleName = "ELL";
    settings.mapFunctionName = "ELL_Predict";
    settings.compilerSettings.useBlas = true;
    settings.compilerSettings.optimize = false;
    settings.profile = false;
    settings.compilerSettings.targetDevice.deviceName = "host";

    // refine 1 level deep to get the layer nodes so we can inject the DebugSinkNodes between each layer of the neural net.
    model::TransformContext context{ [](const model::Node& node) { return node.IsCompilable() ? model::NodeAction::compile : model::NodeAction::refine; } };
    reference.Refine(context, 1);
    reference.Transform(DebugTransformer, context);

    model::IRMapCompiler compiler(settings);
    // grab a pointer to the module before TransferOwnership nulls it out.
    llvm::Module* module = compiler.GetModule().GetLLVMModule();
    std::cout << "compiling..." << std::endl;

    model::IRCompiledMap compiledMap = compiler.Compile(reference);

    // windows can not do automatic resolution of symbols, so it won't find my DebugOutput function above unless I help it here.
    auto func = module->getFunction("DebugOutput");
    compiledMap.GetJitter().DefineFunction(func, reinterpret_cast<uint64_t>(&DebugOutput));

    std::cout << "jitting..." << std::endl;
    compiledMap.FinishJitting();

    std::cout << "executing..." << std::endl;

    // build the graph
    CreateGraph(compiledMap.GetModel());

    _runningCompiled = false;
    _outputReference = _referenceMap.Compute<float>(input);

    // gather the reference model DebugNodeSinks
    std::vector<const ell::model::Node*> referenceNodes;
    for (size_t i = 0, length = _layers.size(); i < length; i++)
    {
        LayerCaptureData* c = _layers[i];
        if (c->referenceDebugNode != nullptr)
        {
            referenceNodes.push_back(c->referenceDebugNode);
        }
    }

    // Now the normal reference.Compute will skip my DebugSinkNodes, so now I have to do another VisitSubset to gather that output.
    // This will cause the DebugSinkNode Sink function to fire which will call AddLayer below.
    auto compute = [](const model::Node& node) { node.Compute(); };
    _referenceMap.GetModel().VisitSubset(referenceNodes, compute);

    _runningCompiled = true;

    // compute compiled output
    _outputCompiled = compiledMap.Compute<float>(input);
}

void ModelComparison::SaveOutput(std::string name, std::vector<float> reference, std::vector<float> compiled)
{
    std::string fileSafeId = name;
    DgmlGraph::ReplaceAll(fileSafeId, "<", "_");
    DgmlGraph::ReplaceAll(fileSafeId, ">", "_");

    std::string fullPath = _outputDirectory;
    if (fullPath != "")
    {
#ifdef WIN32
        fullPath += "\\";
#else
        fullPath += "/";
#endif
    }

    std::ofstream data(fullPath + fileSafeId + ".csv");
    for (size_t i = 0, length = (size_t)fmin(reference.size(), compiled.size()); i < length; i++)
    {
        data << reference[i] << "," << compiled[i] << std::endl;
    }
}

size_t ModelComparison::GetOutputSize(const std::string& id)
{
    return _outputSizes[id];
}

void ModelComparison::WriteReport(std::ostream& outputStream)
{
    std::cout << "comparing..." << std::endl;
    WriteRow(outputStream, "", "Overall", _outputReference, _outputCompiled, nullptr);

    for (auto ptr = _layers.begin(), end = _layers.end(); ptr != end; ptr++)
    {
        LayerCaptureData* c = *ptr;
        if (c->CompiledNodeLabel != "")
        {
            std::string id = c->CompiledNodeId;
            std::string label = c->CompiledNodeLabel;
            WriteRow(outputStream, id, label, c->Reference, c->Compiled, c);
        }
        else
        {
            //std::cout << "Compiled data missing for reference node " << c.ReferenceNodeLabel << std::endl;
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

void ModelComparison::WriteRow(std::ostream& outputStream, std::string id, std::string name, std::vector<float>& reference, std::vector<float>& compiled, LayerCaptureData* c)
{
    if (compiled.size() == 0)
    {
        // Layer was pruned from compiled model
        return;
    }

    SaveOutput("Compare_" + name, _outputReference, _outputCompiled);

    outputStream << "## " << name << std::endl;

    VectorStats refStats(reference);
    VectorStats compiledStats(compiled);
    float diff = VectorStats::Diff(reference, compiled);
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

    outputStream << "````" << std::endl;
    if (c != nullptr)
    {
        outputStream << "size=" << to_string(c->size) << ", stride=" << to_string(c->stride) << ", offset=" << to_string(c->offset) << std::endl;
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
    <Style TargetType="Node" GroupLabel="Error" ValueLabel="Gradient">
      <Condition Expression="Error &gt; 0" />
      <Setter Property="Background" Expression="Color.FromRgb(180 * Math.Max(1, (80 - Coverage) / 30), 180, 0)" />
    </Style>
    */
    
    double range = _maxError - _minError;

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
    red.Expression = "Color.FromRgb(55 + 200 * ( Error + " + std::to_string(_minError) + ") / " + std::to_string(range) + ", 0, 0)";
    es.Setters.push_back(red);

    // add nice styles to show the errors.
    _graph.AddStyle(es);
}

void ModelComparison::AddLayer(const char* label, const float* output)
{
    std::string id(label);
    size_t size = GetOutputSize(id);
    std::vector<float> data(size, *output);
    if (_runningCompiled)
    {
        for (auto ptr = _layers.begin(), end = _layers.end(); ptr != end; ptr++)
        {
            LayerCaptureData* c = *ptr;
            std::string compiledId = _nodeMap[c->ReferenceNodeLabel];
            if (compiledId == id)
            {
                c->Compiled = data;
                break;
            }
        }
    }
    else
    {
        for (auto ptr = _layers.begin(), end = _layers.end(); ptr != end; ptr++)
        {
            LayerCaptureData* c = *ptr;
            if (c->ReferenceNodeLabel == id)
            {
                c->Reference = data;
                break;
            }
        }
    }
}

void ModelComparison::CreateGraph(const model::Model& model)
{
    this->model = &model;

    // Create DGML graph of model
    model.Visit([&](const model::Node& node) {
        std::string typeName = node.GetRuntimeTypeName();
        if (typeName == "DebugSinkNode<float>")
        {
            // during the process of compilation the DynamicMap Model is cloned a few times which
            // causes the node id's to change, so this creates a map from the original id to the new id
            const nodes::DebugSinkNode<float>& debugNode = dynamic_cast<const nodes::DebugSinkNode<float>&>(node);
            auto oldLabel = debugNode.GetLabel();
            int start = oldLabel.find("(");
            start++;
            int end = oldLabel.find(")");
            auto oldId = oldLabel.substr(start, end - start);
            for (auto parentNode : debugNode.GetInputPort("input")->GetParentNodes())
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
                    std::string id = to_string(upstream->GetId());
                    std::string typeName = upstream->GetRuntimeTypeName();
                    if (typeName != "DebugSinkNode<float>")
                    {
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

void ModelComparison::AddDebugOutputNode(model::ModelTransformer& transformer, const nodes::NeuralNetworkLayerNodeBase<float>* layerNode)
{
    // todo: add additional argument to the sink function telling user which layer this is...
    std::string sinkFunctionName = "DebugOutput";
    auto newPortElements = transformer.GetCorrespondingOutputs(layerNode->output);
    std::string label = layerNode->GetRuntimeTypeName() + "(" + to_string(layerNode->GetId()) + ")";

    size_t size = layerNode->GetOutputSize();
    _outputSizes[label] = size;

    //std::cout << "adding debug sink node " << label << ", intput size = " << size << std::endl;

    auto newNode = transformer.AddNode<ell::nodes::DebugSinkNode<float>>(
        newPortElements, [&](const std::string& label, const std::vector<float>& output) {
            AddLayer(label.c_str(), &output[0]);
        },
        label,
        sinkFunctionName);

    if (_addingReference)
    {
        LayerCaptureData* c = new LayerCaptureData();
        c->compiledDebugNode = nullptr;
        c->referenceDebugNode = dynamic_cast<const ell::nodes::DebugSinkNode<float>*>(newNode);
        c->ReferenceNodeLabel = label;
        _layers.push_back(c);
        _nextIndex = 0;
    }
    else
    {
        int i = _nextIndex++;
        if (i < _layers.size())
        {
            LayerCaptureData* c = _layers[i];
            c->compiledDebugNode = dynamic_cast<const ell::nodes::DebugSinkNode<float>*>(newNode);
            auto memLayout = layerNode->GetInputMemoryLayout();
            c->size = memLayout.size;
            c->stride = memLayout.stride;
            c->offset = memLayout.offset;
            c->CompiledNodeId = to_string(layerNode->GetId());
            c->CompiledNodeLabel = label;
            _nodeMap[c->referenceDebugNode->GetLabel()] = label;
        }
    }
}

uint64_t FunctionResolver(const std::string& name)
{
    if (name == "DebugOutput")
    {
        return reinterpret_cast<uint64_t>(&DebugOutput);
    }
    return 0;
}

void DebugTransformer(const ell::model::Node& node, ell::model::ModelTransformer& transformer)
{
    node.Copy(transformer);
    const nodes::NeuralNetworkLayerNodeBase<float>* layerNode = dynamic_cast<const nodes::NeuralNetworkLayerNodeBase<float>*>(&node);
    if (layerNode != nullptr)
    {
        ModelComparison::GetInstance()->AddDebugOutputNode(transformer, layerNode);
    }
}
