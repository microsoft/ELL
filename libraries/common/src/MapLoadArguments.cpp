////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapLoadArguments.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapLoadArguments.h"

// model
#include "Node.h"
#include "Model.h"
#include "DynamicMap.h"

// utilities
#include "Files.h"
#include "Tokenizer.h"

// stl
#include <sstream>

namespace emll
{
namespace common
{
    //
    // MapLoadArguments
    //
    void ParsedMapLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputMapFile,
            "inputMapFile",
            "imap",
            "Path to the input map file",
            "");

        parser.AddOption(
            inputModelFile,
            "inputModelFile",
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
    }

    namespace
    {
        // forward declaration
        model::PortRange ParseRange(const model::Model& model, utilities::Tokenizer& tokenizer);

        std::vector<model::PortRange> ParseRangeList(const model::Model& model, utilities::Tokenizer& tokenizer)
        {
            std::vector<model::PortRange> result;
            while (true)
            {
                // read a range
                result.push_back(ParseRange(model, tokenizer));
                if (tokenizer.PeekNextToken() != ",")
                {
                    break;
                }
                tokenizer.MatchToken(",");
            }
            return result;
        }

        model::PortRange ParseRange(const model::Model& model, utilities::Tokenizer& tokenizer)
        {
            auto nodeId = tokenizer.ReadNextToken();
            tokenizer.MatchToken(".");
            auto portName = tokenizer.ReadNextToken();

            // get port
            auto node = model.GetNode(utilities::UniqueId(nodeId));
            assert(node != nullptr);
            if(node == nullptr)
                throw utilities::InputException(utilities::InputExceptionErrors::nullReference, std::string("Couldn't find node ") + nodeId);
            auto port = node->GetOutputPort(portName);

            // now check for element/element slice
            if (tokenizer.PeekNextToken() == "[")
            {
                tokenizer.MatchToken("[");
                model::PortRange result;
                auto token = tokenizer.ReadNextToken();
                auto startIndex = std::stoi(token);
                if (tokenizer.PeekNextToken() == ":")
                {
                    tokenizer.MatchToken(":");
                    auto endIndex = std::stoi(tokenizer.ReadNextToken());
                    result = model::PortRange(*port, startIndex, endIndex - startIndex);
                }
                else
                {
                    result = model::PortRange(*port, startIndex);
                }

                // number
                // (colon, number)
                tokenizer.MatchToken("]");
                return result;
            }
            else
            {
                return model::PortRange(*port);
            }
        }

        model::PortElementsBase ParsePortElements(const model::Model& model, utilities::Tokenizer& tokenizer) // start state
        {
            auto t = tokenizer.PeekNextToken();
            // TODO: if text is just a number, take it as the index of the input node
            if (t == "{")
            {
                tokenizer.MatchToken(t);
                auto ranges = ParseRangeList(model, tokenizer);
                tokenizer.MatchToken("}");
                return model::PortElementsBase(ranges);
            }
            else
            {
                return model::PortElementsBase(ParseRange(model, tokenizer));
            }
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
        std::stringstream stream(modelOutputsString);
        std::string delimiters = "{}[],.:";
        utilities::Tokenizer tokenizer(stream, delimiters);
        // set up
        return ParsePortElements(model, tokenizer);
    }

    //
    // ParsedMapLoadArguments
    //
    utilities::CommandLineParseResult ParsedMapLoadArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        std::vector<std::string> parseErrorMessages;

        if (inputModelFile != "" && inputMapFile != "")
        {
            parseErrorMessages.push_back("Can only specify an input map or model file, not both.");
        }
        else if (inputMapFile != "")
        {
            if (!utilities::IsFileReadable(inputMapFile))
            {
                parseErrorMessages.push_back("Cannot read from specified input map file: " + inputMapFile);
            }
        }
        else if (inputModelFile != "")
        {
            if (!utilities::IsFileReadable(inputModelFile))
            {
                parseErrorMessages.push_back("Cannot read from specified input model file: " + inputModelFile);
            }
        }

        return parseErrorMessages;
    }
}
}
