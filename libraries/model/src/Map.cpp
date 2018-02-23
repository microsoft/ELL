////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Map.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Map.h"
#include "Exception.h"
#include "ModelOptimizer.h"
#include "ModelTransformer.h"
#include "OutputNode.h"

// stl
#include <algorithm>
#include <unordered_set>

#include <iomanip>
#include <iostream>

namespace ell
{
namespace model
{
    namespace
    {
        //
        // Relevant archive format versions
        //
        constexpr utilities::ArchiveVersion noMetadataArchiveVersion = { utilities::ArchiveVersionNumbers::v2 };
        constexpr utilities::ArchiveVersion metadataArchiveVersion = { utilities::ArchiveVersionNumbers::v3_model_metadata };
    }

    Map::Map(const Model& model, const std::vector<std::pair<std::string, InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, PortElementsBase>>& outputs)
    {
        TransformContext context;
        ModelTransformer transformer;
        _model = transformer.CopyModel(model, context);

        for (const auto& input : inputs)
        {
            AddInput(input.first, input.second);
        }

        for (const auto& output : outputs)
        {
            AddOutput(output.first, output.second);
        }

        // Important: we don't need to call FixTransformedIO here because we do it in the call to Prune
        // FixTransformedIO(transformer);
        Prune();
    }

    Map::Map(const Map& other)
    {
        TransformContext context;
        ModelTransformer transformer;
        _model = transformer.CopyModel(other._model, context);
        for (const auto& input : other._inputNodeMap)
        {
            AddInput(input.first, input.second);
        }

        for (const auto& output : other._outputElementsMap)
        {
            AddOutput(output.first, output.second);
        }

        FixTransformedIO(transformer);
    }

    Map& Map::operator=(Map other)
    {
        swap(*this, other);
        return *this;
    }

    void Map::SetNodeInput(InputNode<bool>* node, const std::vector<bool>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    void Map::SetNodeInput(InputNode<int>* node, const std::vector<int>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    void Map::SetNodeInput(InputNode<int64_t>* node, const std::vector<int64_t>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    void Map::SetNodeInput(InputNode<float>* node, const std::vector<float>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    void Map::SetNodeInput(InputNode<double>* node, const std::vector<double>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    std::vector<bool> Map::ComputeBoolOutput(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<bool>(outputs);
    }

    std::vector<int> Map::ComputeIntOutput(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<int>(outputs);
    }

    std::vector<int64_t> Map::ComputeInt64Output(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<int64_t>(outputs);
    }

    std::vector<float> Map::ComputeFloatOutput(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<float>(outputs);
    }

    std::vector<double> Map::ComputeDoubleOutput(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<double>(outputs);
    }

    template <>
    std::vector<bool> Map::ComputeOutput<bool>(const PortElementsBase& elements) const
    {
        return ComputeBoolOutput(elements);
    }

    template <>
    std::vector<int> Map::ComputeOutput<int>(const PortElementsBase& elements) const
    {
        return ComputeIntOutput(elements);
    }

    template <>
    std::vector<int64_t> Map::ComputeOutput<int64_t>(const PortElementsBase& elements) const
    {
        return ComputeInt64Output(elements);
    }

    template <>
    std::vector<float> Map::ComputeOutput<float>(const PortElementsBase& elements) const
    {
        return ComputeFloatOutput(elements);
    }

    template <>
    std::vector<double> Map::ComputeOutput<double>(const PortElementsBase& elements) const
    {
        return ComputeDoubleOutput(elements);
    }

    void Map::AddInput(const std::string& inputName, InputNodeBase* inputNode)
    {
        _inputNodes.push_back(inputNode);
        _inputNames.push_back(inputName);
        _inputNodeMap.insert({ inputName, inputNode });
    }

    void Map::AddOutput(const std::string& outputName, PortElementsBase outputElements)
    {
        _outputElements.push_back(outputElements);
        _outputNames.push_back(outputName);
        _outputElementsMap.insert({ outputName, outputElements });
    }

    void Map::ResetOutput(size_t index, PortElementsBase outputElements)
    {
        assert(index <= _outputElements.size() && "Error: Resetting unset output");
        _outputElements[index] = outputElements;
        _outputElementsMap[_outputNames[index]] = outputElements;
    }

    void swap(Map& a, Map& b)
    {
        using std::swap;
        swap(a._model, b._model);
        swap(a._inputNodes, b._inputNodes);
        swap(a._inputNames, b._inputNames);
        swap(a._inputNodeMap, b._inputNodeMap);
        swap(a._outputElements, b._outputElements);
        swap(a._outputNames, b._outputNames);
        swap(a._outputElementsMap, b._outputElementsMap);
    }

    std::vector<const Node*> Map::GetAllOutputNodes() const
    {
        // gather output nodes
        std::unordered_set<const Node*> outputNodes;
        for (const auto& output : GetOutputs())
        {
            for (const auto& range : output.GetRanges())
            {
                outputNodes.insert(range.ReferencedPort()->GetNode());
            }
        }

        return { outputNodes.begin(), outputNodes.end() };
    }

    std::vector<const Node*> Map::GetDebugSinkNodes() const
    {
        // gather SinkNodes
        std::unordered_set<const Node*> sinkNodes;
        _model.Visit([&](const Node& node) {
            if (node.GetRuntimeTypeName().find("DebugSinkNode") != std::string::npos)
            {
                auto parents = node.GetParentNodes();
                for (auto ptr = parents.begin(), end = parents.end(); ptr != end; ptr++)
                {
                    const Node* parent = *ptr;
                    auto dependents = parent->GetDependentNodes();
                    for (auto ptr2 = dependents.begin(), end2 = dependents.end(); ptr2 != end2; ptr2++)
                    {
                        const Node* dep = *ptr2;
                        if (dep != &node)
                        {
                            // then we want to keep this DebugSinkNode, it should not get pruned.
                            sinkNodes.insert(&node);
                            break;
                        }
                    }
                }
            }
        });
        return { sinkNodes.begin(), sinkNodes.end() };
    }

    void Map::FixTransformedIO(ModelTransformer& transformer)
    {
        for (auto& inputNode : _inputNodes)
        {
            auto refinedInput = transformer.GetCorrespondingInputNode(inputNode);
            inputNode = refinedInput;
        }

        for (auto& inputNode : _inputNodeMap)
        {
            auto input = inputNode.second;
            auto refinedInput = transformer.GetCorrespondingInputNode(input);
            inputNode.second = refinedInput;
        }

        for (auto& outputElements : _outputElements)
        {
            auto refinedOutput = transformer.GetCorrespondingOutputs(outputElements);
            outputElements = refinedOutput;
        }

        for (auto& outputElements : _outputElementsMap)
        {
            auto output = outputElements.second;
            auto refinedOutput = transformer.GetCorrespondingOutputs(output);
            outputElements.second = refinedOutput;
        }
    }

    void Map::FixTransformedIO(ModelOptimizer& optimizer)
    {
        for (auto& inputNode : _inputNodes)
        {
            auto refinedInput = optimizer.GetCorrespondingInputNode(inputNode);
            inputNode = refinedInput;
        }

        for (auto& inputNode : _inputNodeMap)
        {
            auto input = inputNode.second;
            auto refinedInput = optimizer.GetCorrespondingInputNode(input);
            inputNode.second = refinedInput;
        }

        for (auto& outputElements : _outputElements)
        {
            auto refinedOutput = optimizer.GetCorrespondingOutputs(outputElements);
            outputElements = refinedOutput;
        }

        for (auto& outputElements : _outputElementsMap)
        {
            auto output = outputElements.second;
            auto refinedOutput = optimizer.GetCorrespondingOutputs(output);
            outputElements.second = refinedOutput;
        }
    }

    void Map::Prune()
    {
        TransformContext context;
        ModelTransformer transformer;

        auto outputNodeVec = GetAllOutputNodes();
        auto sinkNodes = GetDebugSinkNodes();
        outputNodeVec.insert(outputNodeVec.end(), sinkNodes.begin(), sinkNodes.end());
        auto minimalModel = transformer.CopyModel(_model, outputNodeVec, context);
        FixTransformedIO(transformer);
        _model = std::move(minimalModel);
    }

    size_t Map::GetInputSize() const
    {
        return GetInputShape().Size();
    }

    size_t Map::GetOutputSize() const
    {
        return GetOutput(0).Size();
    }

    math::TensorShape Map::GetInputShape() const
    {
        auto sourceNodes = _model.GetNodesByType<SourceNodeBase>();
        if (!sourceNodes.empty())
        {
            return sourceNodes[0]->GetShape();
        }

        // no source nodes, fallback to first input node's shape
        return GetInput(0)->GetShape();
    }

    std::vector<const InputNodeBase*> Map::GetInputNodes() const
    {
        auto sourceNodes = _model.GetNodesByType<SourceNodeBase>();
        if (!sourceNodes.empty())
        {
            return std::vector<const InputNodeBase*>(sourceNodes.begin(), sourceNodes.end());
        }

        // no source nodes, fallback to regular input nodes
        return std::vector<const InputNodeBase*>(_inputNodes.begin(), _inputNodes.end());
    }

    std::vector<const OutputNodeBase*> Map::GetOutputNodes() const
    {
        std::vector<const OutputNodeBase*> result;
        auto allOutputs = GetAllOutputNodes();
        for (auto iter = allOutputs.begin(), end = allOutputs.end(); iter != end; iter++)
        {
            const Node* node = *iter;
            const OutputNodeBase* outputNode = dynamic_cast<const OutputNodeBase*>(node);
            if (outputNode != nullptr)
            {
                result.push_back(outputNode);
            }
        }
        return result;
    }

    math::TensorShape Map::GetOutputShape() const
    {
        auto outputNodeVec = GetOutputNodes();
        if (!outputNodeVec.empty())
        {
            const OutputNodeBase* node = dynamic_cast<const OutputNodeBase*>(outputNodeVec[0]);
            return node->GetShape();
        }
        return math::TensorShape(0, 0, 0);
    }

    Port::PortType Map::GetInputType() const
    {
        auto sourceNodes = _model.GetNodesByType<SourceNodeBase>();
        if (!sourceNodes.empty())
        {
            return sourceNodes[0]->GetOutputType();
        }
        
        return GetInput()->GetOutputType();
    }

    Port::PortType Map::GetOutputType() const
    {
        return GetOutput().GetPortType();
    }

    void Map::Refine(int maxIterations)
    {
        TransformContext context;
        return Refine(context, maxIterations);
    }

    void Map::Refine(const TransformContext& context, int maxIterations)
    {
        if (maxIterations == 0)
        {
            return;
        }

        ModelTransformer transformer;
        auto refinedModel = transformer.RefineModel(_model, context, maxIterations);
        FixTransformedIO(transformer);

        _model = std::move(refinedModel);
        Prune();
    }

    void Map::Transform(const std::function<void(const Node&, ModelTransformer&)>& transformFunction, const TransformContext& context)
    {
        ModelTransformer transformer;
        auto refinedModel = transformer.TransformModel(_model, context, transformFunction);
        FixTransformedIO(transformer);
        _model = std::move(refinedModel);
    }

    void Map::Optimize(ModelOptimizer& optimizer)
    {
        auto optimizedModel = optimizer.OptimizeModel(_model);
        FixTransformedIO(optimizer);
        _model = std::move(optimizedModel);
        Prune();
    }

    void Map::RenameCallbacks(const std::string& sourceCallbackName, const std::string& sinkCallbackName)
    {
        // Look for all source nodes and apply name if it is non-empty.
        // The callbacks will soon support a context parameter that indicates the originating source node.
        if (!sourceCallbackName.empty())
        {
            auto nodes = _model.GetNodesByType<SourceNodeBase>();
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                nodes[i]->SetCallbackName(sourceCallbackName);
            }
        }

        // Look for all sink nodes and apply name if it is non-empty.
        // The callbacks will soon support a context parameter that indicates the originating sink node.
        if (!sinkCallbackName.empty())
        {
            auto nodes = _model.GetNodesByType<SinkNodeBase>();
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                nodes[i]->SetCallbackName(sinkCallbackName);
            }
        }
    }

    utilities::ArchiveVersion Map::GetArchiveVersion() const
    {
        if (_metadata.IsEmpty())
        {
            return noMetadataArchiveVersion;
        }
        else
        {
            return metadataArchiveVersion;
        }
    }

    bool Map::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return version >= noMetadataArchiveVersion && version <= metadataArchiveVersion;
    }

    void Map::WriteToArchive(utilities::Archiver& archiver) const
    {
        // Archive the model
        archiver["model"] << _model;

        // Archive the inputs
        std::vector<utilities::UniqueId> inputIds(_inputNodes.size());
        std::transform(_inputNodes.begin(), _inputNodes.end(), inputIds.begin(), [](InputNodeBase* node) { return node->GetId(); });
        archiver["inputNames"] << _inputNames;
        archiver["inputIds"] << inputIds;

        // Archive the outputs
        archiver["outputNames"] << _outputNames;
        archiver["outputElements"] << _outputElements;
    }

    void Map::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        MapSerializationContext mapContext(archiver.GetContext());
        archiver.PushContext(mapContext);

        // Unarchive the model
        archiver["model"] >> _model;

        // Unarchive the inputs
        std::vector<utilities::UniqueId> inputIds;
        archiver["inputNames"] >> _inputNames;
        archiver["inputIds"] >> inputIds;

        // Unarchive the outputs
        archiver["outputNames"] >> _outputNames;
        archiver["outputElements"] >> _outputElements;

        // Reconstruct the input node map
        _inputNodeMap.clear();
        _inputNodes.resize(inputIds.size());
        assert(_inputNames.size() == inputIds.size());
        for (size_t index = 0; index < _inputNames.size(); ++index)
        {
            auto node = mapContext.GetNodeFromSerializedId(inputIds[index]);
            assert(dynamic_cast<InputNodeBase*>(node) != nullptr);
            _inputNodes[index] = static_cast<InputNodeBase*>(node);
            _inputNodeMap[_inputNames[index]] = static_cast<InputNodeBase*>(node);
        }

        // Reconstruct the output elements map
        _outputElementsMap.clear();
        assert(_outputNames.size() == _outputElements.size());
        for (size_t index = 0; index < _outputNames.size(); ++index)
        {
            _outputElementsMap[_outputNames[index]] = _outputElements[index];
        }

        archiver.PopContext();
    }

    InputNodeBase* Map::GetInput(size_t index) const
    {
        if (index >= _inputNodes.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return _inputNodes[index];
    }

    InputNodeBase* Map::GetInput(const std::string& inputName) const
    {
        auto iter = _inputNodeMap.find(inputName);
        if (iter == _inputNodeMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return iter->second;
    }

    PortElementsBase Map::GetOutput(size_t index) const
    {
        if (index >= _outputElements.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return _outputElements[index];
    }

    PortElementsBase Map::GetOutput(const std::string& outputName) const
    {
        auto iter = _outputElementsMap.find(outputName);
        if (iter == _outputElementsMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return iter->second;
    }

    //
    // MapSerializationContext
    //
    MapSerializationContext::MapSerializationContext(utilities::SerializationContext& previousContext)
        : ModelSerializationContext(previousContext, nullptr)
    {
    }
}
}
