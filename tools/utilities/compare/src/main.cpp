////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (compare)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompareArguments.h"

// common
#include "LoadModel.h"
#include "ModelLoadArguments.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "ModelComparison.h"
#include "OutputStreamImpostor.h"
#include "Files.h"

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

        // load map file
        std::cout << "loading map..." << std::endl;
        model::DynamicMap map = common::LoadMap(compareArguments.inputMapFile);

        // bugbug: have to assume input is square, since map seems to be losing this information.
        double size = map.GetInputSize() / 3;
        int rows = (int)sqrt(size);
        int cols = rows;
        std::vector<float> input = ResizeImage(compareArguments.inputTestFile, rows, cols);

        ModelComparison comparison(compareArguments.outputDirectory);
        comparison.Compare(input, map);

        std::string reportFileName = ell::utilities::JoinPaths(compareArguments.outputDirectory, "report.md");

        {
            std::ofstream reportStream(reportFileName, std::ios::out);
            comparison.WriteReport(reportStream);
        }
        // output annotated graph showing where differences occurred in the model between compiled and reference implementation.
        std::string graphFileName = ell::utilities::JoinPaths(compareArguments.outputDirectory, "graph.dgml");
        {

            std::ofstream graphStream(graphFileName, std::ios::out);
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
