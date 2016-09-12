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

        std::vector<std::string> inputNames;
        std::vector<utilities::UniqueId> inputIds;
        for(const auto& input: _inputNodeMap)
        {
            inputNames.push_back(input.first);
            inputIds.push_back(input.second->GetId());
        }
        archiver["inputNames"] << inputNames;
        archiver["inputIds"] << inputIds;
    }

    void DynamicMap::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["model"] >> _model;
        std::vector<std::string> inputNames;
        std::vector<utilities::UniqueId> inputIds;
        archiver["inputNames"] >> inputNames;
        archiver["inputIds"] >> inputIds;
    }
}
}
