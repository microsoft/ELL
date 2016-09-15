////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DynamicMap.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DynamicMap.h"
#include "ModelTransformer.h"
#include "Exception.h"

namespace emll
{
namespace model
{
    DynamicMap::DynamicMap(const Model& model, const std::vector<std::pair<std::string, InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, PortElementsBase>>& outputs)
        : _model(model)
    {
        for (const auto& input : inputs)
        {
            _inputNodeMap[input.first] = input.second;
        }

        for (const auto& output : outputs)
        {
            _outputElementsMap[output.first] = output.second;
        }
    }

    void DynamicMap::Refine(const TransformContext& context)
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
    }

    void DynamicMap::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["model"] << _model;

        // Need to do outputs, too!
        std::vector<std::string> inputNames;
        std::vector<utilities::UniqueId> inputIds;
        for (const auto& input : _inputNodeMap)
        {
            inputNames.push_back(input.first);
            inputIds.push_back(input.second->GetId());
        }
        archiver["inputNames"] << inputNames;
        archiver["inputIds"] << inputIds;

        // Need to do outputs, too!
        std::vector<std::string> outputNames;
        std::vector<PortElementsBase> outputElements;
        for (const auto& output : _outputElementsMap)
        {
            outputNames.push_back(output.first);
            outputElements.push_back(output.second);
        }
        archiver["outputNames"] << outputNames;
        archiver["outputElements"] << outputElements;
    }

    void DynamicMap::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        DynamicMapSerializationContext mapContext(archiver.GetContext());
        archiver.PushContext(mapContext);

        archiver["model"] >> _model;
        std::vector<std::string> inputNames;
        std::vector<utilities::UniqueId> inputIds;
        archiver["inputNames"] >> inputNames;
        archiver["inputIds"] >> inputIds;

        std::vector<std::string> outputNames;
        std::vector<PortElementsBase> outputElements;
        archiver["outputNames"] >> outputNames;
        archiver["outputElements"] >> outputElements;

        // reconstruct _inputNodeMap
        _inputNodeMap.clear();
        assert(inputNames.size() == inputIds.size());
        for (size_t index = 0; index < inputNames.size(); ++index)
        {
            auto node = mapContext.GetNodeFromSerializedId(inputIds[index]);
            assert(dynamic_cast<InputNodeBase*>(node) != nullptr);
            _inputNodeMap[inputNames[index]] = static_cast<InputNodeBase*>(node);
        }

        // reconstruct _outputElementsMap
        _outputElementsMap.clear();
        assert(outputNames.size() == outputElements.size());
        for (size_t index = 0; index < outputNames.size(); ++index)
        {
            _outputElementsMap[outputNames[index]] = outputElements[index];
        }

        archiver.PopContext();
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
