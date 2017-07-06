////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapLoadArguments.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapLoadArguments.h"

// model
#include "DynamicMap.h"
#include "Model.h"
#include "Node.h"

// utilities
#include "Files.h"
#include "Tokenizer.h"

// stl
#include <sstream>

namespace ell
{
namespace common
{
    //
    // MapLoadArguments
    //
    void ParsedMapLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputMapFilename,
            "inputMapFilename",
            "imap",
            "Path to the input map file",
            "");

        parser.AddOption(
            inputModelFilename,
            "inputModelFilename",
            "imf",
            "Path to the input model file",
            "");

        parser.AddOption(
            modelInputsString,
            "modelInputs",
            "in",
            "Model inputs to use",
            "");

        parser.AddOption(
            modelOutputsString,
            "modelOutputs",
            "out",
            "Model outputs to use",
            "");

        parser.AddOption(
            defaultInputSize,
            "defaultInputSize",
            "d",
            "Default size of input node",
            1);

        parser.AddOption(
            mapType,
            "mapType",
            "m",
            "Choice of map type (must match input map file): simple, clockStep, sysClockStep",
            { { "simple", MapType::simpleMap }, { "clockStep", MapType::steadyClockSteppableMap }, { "sysClockStep", MapType::systemClockSteppableMap } },
            "simple");
    }

    std::string MapLoadArguments::GetInputFilename() const
    {
        if (HasMapFilename())
        {
            return inputMapFilename;
        }
        else if (HasModelFilename())
        {
            return inputModelFilename;
        }
        else
        {
            return "";
        }
    }

    model::InputNodeBase* MapLoadArguments::GetInput(model::Model& model) const
    {
        // name of input node
        auto inputNode = model.GetNode(utilities::UniqueId(modelInputsString));
        return dynamic_cast<model::InputNodeBase*>(inputNode);
    }

    model::PortElementsBase MapLoadArguments::GetOutput(model::Model& model) const
    {
        auto elementsProxy = model::ParsePortElementsProxy(modelOutputsString);
        return model::ProxyToPortElements(model, elementsProxy);
    }

    //
    // ParsedMapLoadArguments
    //
    utilities::CommandLineParseResult ParsedMapLoadArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        std::vector<std::string> parseErrorMessages;

        if (inputModelFilename != "" && inputMapFilename != "")
        {
            parseErrorMessages.push_back("Can only specify an input map or model file, not both.");
        }
        else if (inputMapFilename != "")
        {
            if (!utilities::IsFileReadable(inputMapFilename))
            {
                parseErrorMessages.push_back("Cannot read from specified input map file: " + inputMapFilename);
            }
        }
        else if (inputModelFilename != "")
        {
            if (!utilities::IsFileReadable(inputModelFilename))
            {
                parseErrorMessages.push_back("Cannot read from specified input model file: " + inputModelFilename);
            }
        }

        return parseErrorMessages;
    }
}
}
