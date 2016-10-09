////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (apply)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// test parameters
// bin\Release\apply -idf examples/data/testData.txt --inputModelFile examples/data/model_1.json -v -in 3128 -out 3133.output

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

// data
#include "Example.h"
#include "Dataset.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "DataSaveArguments.h"
#include "LoadModel.h"
#include "MapLoadArguments.h"
#include "MapSaveArguments.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>

using namespace emll;

// helper function
void SplitString(const std::string& str, char delimiter, std::vector<std::string>& elements)
{
    std::stringstream stream(str);
    std::string element;
    while (getline(stream, element, delimiter))
    {
        elements.push_back(element);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedDataSaveArguments dataSaveArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedMapSaveArguments mapSaveArguments;

        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(dataSaveArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);

        bool verbose = false;
        commandLineParser.AddOption(verbose, "verbose", "v", "Verbose mode", false);

        // parse command line
        commandLineParser.Parse();

        if (verbose)
        {
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load data set
        if (verbose) std::cout << "Loading data from file: " << dataLoadArguments.inputDataFilename << std::endl;
//TODO        auto dataset = common::GetDataset<data::DenseDataset>(dataLoadArguments);
// TODO - consider removing        size_t numColumns = dataLoadArguments.parsedDataDimension;

        // load map
        model::DynamicMap map;
        if (mapLoadArguments.HasMapFile())
        {
            if (verbose) std::cout << "Loading map from file: " << mapLoadArguments.inputMapFile << std::endl;
            map = common::LoadMap(mapLoadArguments.inputMapFile);
        }
        else
        {
            if (verbose) std::cout << "Loading model from file: " << mapLoadArguments.inputModelFile << std::endl;
            auto model = common::LoadModel(mapLoadArguments.inputModelFile);

            model::InputNodeBase* inputNode = nullptr;
            model::PortElementsBase outputElements;
            if (mapLoadArguments.modelInputsString != "")
            {
                // for now, modelInputString will just be the ID of an InputNode, and we'll call it 'input'
                auto inputNodeId = utilities::UniqueId(mapLoadArguments.modelInputsString);
                inputNode = dynamic_cast<model::InputNodeBase*>(model.GetNode(inputNodeId));
                if (inputNode == nullptr)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find input node");
                }
            }
            else // look for first input node
            {
                auto inputNodes = model.GetNodesByType<model::InputNodeBase>();
                if (inputNodes.size() == 0)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find input node");
                }
                inputNode = inputNodes[0];
                std::cout << "Using input Node " << inputNode->GetId() << std::endl;
            }

            if (mapLoadArguments.modelOutputsString != "")
            {
                // split string into "node.port"
                std::vector<std::string> outputParts;
                SplitString(mapLoadArguments.modelOutputsString, '.', outputParts);
                auto outputNodeId = utilities::UniqueId(outputParts[0]);
                auto outputPortName = outputParts[1];
                auto outputNode = model.GetNode(outputNodeId);
                if (outputNode == nullptr)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, std::string("Can't find output node ") + to_string(outputNodeId));
                }

                auto outputPort = outputNode->GetOutputPort(outputPortName); // ptr to port base
                if (outputPort == nullptr)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find output port");
                }
                outputElements = model::PortElementsBase(*outputPort);
            }
            else // look for first output node
            {
                auto outputNodes = model.GetNodesByType<model::OutputNodeBase>();
                if (outputNodes.size() == 0)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find output node");
                }
                auto outputNode = outputNodes[0];
                auto outputPorts = outputNode->GetOutputPorts();
                if (outputPorts.size() == 0)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find output port");
                }

                auto outputPort = outputPorts[0]; // ptr to port base
                outputElements = model::PortElementsBase(*outputPort);
                std::cout << "Using output Node " << outputNode->GetId() << "." << outputPort->GetName() << std::endl;
            }

            map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputElements } });
        }

        // Get data set iterator
// TODO        auto datasetIterator = dataset.GetIterator();

        // get output stream
        auto outputStream = dataSaveArguments.outputDataStream;
        auto mapInputSize = map.GetInputSize("input");
// TODO        //while (datasetIterator.IsValid())
        //{
        //    auto row = datasetIterator.Get();
        //    // truncate to size of map input
        //    auto dataVec = row.GetDataVector();
        //    dataVec.Resize(mapInputSize);
        //    map.SetInputValue<double>("input", dataVec);
        //    auto output = map.ComputeOutput<data::DoubleDataVector>("output");
        //    auto mappedRow = data::DenseSupervisedExample{ output, row.GetMetadata() };
        //    mappedRow.Print(outputStream);
        //    outputStream << std::string("\n");
        //    datasetIterator.Next();
        //}

        //// write map to output if desired
        //if(mapSaveArguments.hasOutputStream)
        //{
        //    if(verbose) std::cout << "Saving map to file " << mapSaveArguments.outputMapFile << std::endl;
        //    auto ext = utilities::GetFileExtension(mapSaveArguments.outputMapFile, true);
        //    if(ext == "")
        //    {
        //        ext = "json";
        //    }
        //    common::SaveMap(map, mapSaveArguments.outputMapStream, ext);
        //}
    }
    catch (const utilities::CommandLineParserPrintHelpException& exception)
    {
        std::cout << exception.GetHelpText() << std::endl;
        return 0;
    }
    catch (const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for (const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 1;
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "exception: " << exception.GetMessage() << std::endl;
        return 1;
    }

    return 0;
}
