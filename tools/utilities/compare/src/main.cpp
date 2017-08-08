////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (compare)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompareArguments.h"
#include "CompareUtils.h"
#include "ModelComparison.h"
#include "ResizeImage.h"

// common
#include "LoadModel.h"
#include "ModelLoadArguments.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

// stl
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace ell;

std::vector<float> GetInputImage(std::string filename, const model::DynamicMap& map)
{
    auto inputShape = GetInputShape(map);
    auto outputSize = map.GetOutputSize();
    bool verbose = true;
    if (verbose)
    {
        std::cout << "Model input dimensions: " << inputShape[0] << " x " << inputShape[1] << " x " << inputShape[2] << std::endl;
        std::cout << "Model output size: " << outputSize << std::endl;
    }

     return ResizeImage(filename, inputShape[0], inputShape[1]);
}

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        ParsedCompareArguments compareArguments;
        commandLineParser.AddOptionSet(compareArguments);
        commandLineParser.Parse();

        if(compareArguments.inputMapFile.empty() || compareArguments.inputTestFile.empty())
        {
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 1;
        }

        // load map file
        std::cout << "loading map..." << std::endl;
        model::DynamicMap map = common::LoadMap(compareArguments.inputMapFile);

        auto input = GetInputImage(compareArguments.inputTestFile, map);
        ModelComparison comparison(compareArguments.outputDirectory);
        comparison.Compare(input, map, compareArguments.useBlas, compareArguments.optimize);

        // Write summary report
        if(compareArguments.writeReport)
        {
            std::string reportFileName = utilities::JoinPaths(compareArguments.outputDirectory, "report.md");
            std::ofstream reportStream(reportFileName);
            comparison.WriteReport(reportStream, compareArguments.inputMapFile, compareArguments.inputTestFile);
        }

        // Write an annotated graph showing where differences occurred in the model between compiled and reference implementation.
        if(compareArguments.writeGraph)
        {
            std::string graphFileName = utilities::JoinPaths(compareArguments.outputDirectory, "graph.dgml");
            std::ofstream graphStream(graphFileName);
            comparison.SaveGraph(graphStream);
        }
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
    catch (utilities::LogicException exception)
    {
        std::cerr << "runtime error: " << exception.GetMessage() << std::endl;
        return 1;
    }

    // the end
    return 0;
}
