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

    void DynamicMap::AddInput(const std::string& inputName, InputNodeBase* inputNode)
    {
        std::cout << "AddInput(" << inputName << ")" << std::endl;
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

    size_t DynamicMap::GetInputSize(const std::string& inputName) const
    {
        auto iter = _inputNodeMap.find(inputName);
        return iter->second->GetOutputPort().Size();
    }

    ModelTransformer DynamicMap::Refine(const TransformContext& context)
    {
        ModelTransformer transformer;
        auto refinedModel = transformer.RefineModel(_model, context);

        for (auto& inputNode : _inputNodeMap)
        {
            auto input = inputNode.second;
            auto refinedInput = transformer.GetCorrespondingInputNode(input);
            inputNode.second = refinedInput;
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
        std::vector<utilities::UniqueId> inputIds;

        // Wrong: not it the correct order
        for (size_t index = 0; index < _inputNodes.size(); ++index)
        {
            inputIds.push_back(_inputNodes[index]->GetId());
        }
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

    InputNodeBase* DynamicMap::GetInput(size_t index)
    {
        return _inputNodes[index];
    }

    PortElementsBase DynamicMap::GetOutput(size_t index)
    {
        return _outputElements[index];
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
