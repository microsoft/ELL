////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DynamicMap.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DynamicMap.h"
#include "Exception.h"
#include "ModelTransformer.h"

// stl
#include <algorithm>
#include <unordered_set>

namespace emll
{
namespace model
{
    DynamicMap::DynamicMap(const Model& model)
        : _model(model)
    {
    }

    DynamicMap::DynamicMap(const Model& model, const std::vector<std::pair<std::string, InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, PortElementsBase>>& outputs)
        : _model(model)
    {
        for (const auto& input : inputs)
        {
            AddInput(input.first, input.second);
        }

        for (const auto& output : outputs)
        {
            AddOutput(output.first, output.second);
        }
    }

    void DynamicMap::SetNodeInput(InputNode<bool>* node, const std::vector<bool>& inputValues)
    {
        node->SetInput(inputValues);
    }

    void DynamicMap::SetNodeInput(InputNode<int>* node, const std::vector<int>& inputValues)
    {
        node->SetInput(inputValues);
    }

    void DynamicMap::SetNodeInput(InputNode<double>* node, const std::vector<double>& inputValues)
    {
        node->SetInput(inputValues);
    }

    std::vector<bool> DynamicMap::ComputeBoolOutput(const PortElementsBase& outputs)
    {
        return _model.ComputeOutput<bool>(outputs);
    }

    std::vector<int> DynamicMap::ComputeIntOutput(const PortElementsBase& outputs)
    {
        return _model.ComputeOutput<int>(outputs);
    }

    std::vector<double> DynamicMap::ComputeDoubleOutput(const PortElementsBase& outputs)
    {
        return _model.ComputeOutput<double>(outputs);
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

    InputNodeBase* DynamicMap::GetInputNode(int inputIndex) const
    {
        return _inputNodes[inputIndex];
    }

    InputNodeBase* DynamicMap::GetInputNode(const std::string& inputName) const
    {
        auto iter = _inputNodeMap.find(inputName);
        return iter->second;
    }

   void DynamicMap::ResetOutput(size_t index, PortElementsBase outputElements)
    {
        assert(index > 0 && index <= _outputElements.size() && "Error: Resetting unset output");
        _outputElements[index] = outputElements;
        _outputElementsMap[_outputNames[index]] = outputElements;
    }

    size_t DynamicMap::GetInputSize(int inputIndex) const
    {
        return _inputNodes[inputIndex]->GetOutputPort().Size();
    }

    size_t DynamicMap::GetInputSize(const std::string& inputName) const
    {
        auto iter = _inputNodeMap.find(inputName);
        return iter->second->GetOutputPort().Size();
    }

    size_t DynamicMap::GetOutputSize(int outputIndex) const
    {
        return _outputElements[outputIndex].Size();
    }

    size_t DynamicMap::GetOutputSize(const std::string& outputName) const
    {
        auto iter = _outputElementsMap.find(outputName);
        return iter->second.Size();
    }

    PortElementsBase DynamicMap::GetOutputElementsBase(size_t outputIndex)
    {
        return _outputElements[outputIndex];
    }

    PortElementsBase DynamicMap::GetOutputElementsBase(const std::string& outputName)
    {
        return _outputElementsMap[outputName];
    }

    void DynamicMap::Refine(const TransformContext& context)
    {
        DoRefine(context);
    }

    ModelTransformer DynamicMap::DoRefine(const TransformContext& context)
    {
        ModelTransformer transformer;

        // gather output nodes
        std::unordered_set<const Node*> outputNodes;
        for(const auto& output: GetOutputs())
        {
            for(const auto& range: output.GetRanges())
            {
                outputNodes.insert(range.ReferencedPort()->GetNode());
            }
        }

 		std::vector<const Node*> outputNodeVec(outputNodes.begin(), outputNodes.end());
		auto minimalModel = transformer.CopyModel(_model, outputNodeVec, context);
		auto refinedModel = transformer.RefineModel(minimalModel, context);

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

		_model = refinedModel;
        return transformer;
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
