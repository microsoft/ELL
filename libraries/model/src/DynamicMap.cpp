////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DynamicMap.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DynamicMap.h"
#include "Exception.h"
#include "ModelTransformer.h"

// stl
#include <algorithm>
#include <unordered_set>

#include <iomanip>
#include <iostream>

namespace ell
{
namespace model
{
    DynamicMap::DynamicMap(const Model& model, const std::vector<std::pair<std::string, InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, PortElementsBase>>& outputs)
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

    DynamicMap::DynamicMap(const DynamicMap& other)
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

    DynamicMap& DynamicMap::operator=(DynamicMap other)
    {
        swap(*this, other);
        return *this;
    }

    void DynamicMap::SetNodeInput(InputNode<bool>* node, const std::vector<bool>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    void DynamicMap::SetNodeInput(InputNode<int>* node, const std::vector<int>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    void DynamicMap::SetNodeInput(InputNode<int64_t>* node, const std::vector<int64_t>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    void DynamicMap::SetNodeInput(InputNode<float>* node, const std::vector<float>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    void DynamicMap::SetNodeInput(InputNode<double>* node, const std::vector<double>& inputValues) const
    {
        node->SetInput(inputValues);
    }

    std::vector<bool> DynamicMap::ComputeBoolOutput(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<bool>(outputs);
    }

    std::vector<int> DynamicMap::ComputeIntOutput(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<int>(outputs);
    }

    std::vector<int64_t> DynamicMap::ComputeInt64Output(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<int64_t>(outputs);
    }

    std::vector<float> DynamicMap::ComputeFloatOutput(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<float>(outputs);
    }

    std::vector<double> DynamicMap::ComputeDoubleOutput(const PortElementsBase& outputs) const
    {
        return _model.ComputeOutput<double>(outputs);
    }

    template <>
    std::vector<bool> DynamicMap::ComputeOutput<bool>(const PortElementsBase& elements) const
    {
        return ComputeBoolOutput(elements);
    }

    template <>
    std::vector<int> DynamicMap::ComputeOutput<int>(const PortElementsBase& elements) const
    {
        return ComputeIntOutput(elements);
    }

    template <>
    std::vector<int64_t> DynamicMap::ComputeOutput<int64_t>(const PortElementsBase& elements) const
    {
        return ComputeInt64Output(elements);
    }

    template <>
    std::vector<float> DynamicMap::ComputeOutput<float>(const PortElementsBase& elements) const
    {
        return ComputeFloatOutput(elements);
    }

    template <>
    std::vector<double> DynamicMap::ComputeOutput<double>(const PortElementsBase& elements) const
    {
        return ComputeDoubleOutput(elements);
    }

    void DynamicMap::AddInput(const std::string& inputName, InputNodeBase* inputNode)
    {
        _inputNodes.push_back(inputNode);
        _inputNames.push_back(inputName);
        _inputNodeMap.insert({ inputName, inputNode });
    }

    void DynamicMap::AddOutput(const std::string& outputName, PortElementsBase outputElements)
    {
        _outputElements.push_back(outputElements);
        _outputNames.push_back(outputName);
        _outputElementsMap.insert({ outputName, outputElements });
    }

    void DynamicMap::ResetOutput(size_t index, PortElementsBase outputElements)
    {
        assert(index >= 0 && index <= _outputElements.size() && "Error: Resetting unset output");
        _outputElements[index] = outputElements;
        _outputElementsMap[_outputNames[index]] = outputElements;
    }

    void swap(DynamicMap& a, DynamicMap& b)
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

    std::vector<const Node*> DynamicMap::GetOutputNodes()
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

    void DynamicMap::FixTransformedIO(ModelTransformer& transformer)
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

    void DynamicMap::Prune()
    {
        TransformContext context;
        ModelTransformer transformer;

        auto outputNodeVec = GetOutputNodes();
        auto minimalModel = transformer.CopyModel(_model, outputNodeVec, context);
        FixTransformedIO(transformer);
        _model = std::move(minimalModel);
    }

    size_t DynamicMap::GetInputSize() const
    {
        return GetInput(0)->Size();
    }

    size_t DynamicMap::GetOutputSize() const
    {
        return GetOutput(0).Size();
    }

    Port::PortType DynamicMap::GetInputType() const
    {
        return GetInput()->GetOutputType();
    }

    Port::PortType DynamicMap::GetOutputType() const
    {
        return GetOutput().GetPortType();
    }

    void DynamicMap::Refine(int maxIterations)
    {
        TransformContext context;
        return Refine(context, maxIterations);
    }

    void DynamicMap::Refine(const TransformContext& context, int maxIterations)
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

    void DynamicMap::Transform(const std::function<void(const Node&, ModelTransformer&)>& transformFunction, const TransformContext& context)
    {
        ModelTransformer transformer;
        auto refinedModel = transformer.TransformModel(_model, transformFunction, context);
        FixTransformedIO(transformer);
        _model = std::move(refinedModel);
    }

    void DynamicMap::WriteToArchive(utilities::Archiver& archiver) const
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

    void DynamicMap::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        DynamicMapSerializationContext mapContext(archiver.GetContext());
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

    InputNodeBase* DynamicMap::GetInput(size_t index) const
    {
        if (index >= _inputNodes.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return _inputNodes[index];
    }

    InputNodeBase* DynamicMap::GetInput(const std::string& inputName) const
    {
        auto iter = _inputNodeMap.find(inputName);
        if (iter == _inputNodeMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return iter->second;
    }

    PortElementsBase DynamicMap::GetOutput(size_t index) const
    {
        if (index >= _outputElements.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return _outputElements[index];
    }

    PortElementsBase DynamicMap::GetOutput(const std::string& outputName) const
    {
        auto iter = _outputElementsMap.find(outputName);
        if (iter == _outputElementsMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return iter->second;
    }

    //
    // DynamicMapSerializationContext
    //
    DynamicMapSerializationContext::DynamicMapSerializationContext(utilities::SerializationContext& previousContext)
        : ModelSerializationContext(previousContext, nullptr)
    {
    }
}
}
