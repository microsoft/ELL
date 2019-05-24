////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (print)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintArguments.h"
#include "PrintGraph.h"
#include "PrintModel.h"

#include <common/include/LoadModel.h>
#include <common/include/MapCompilerArguments.h>
#include <common/include/MapLoadArguments.h>

#include <model/include/IRMapCompiler.h>
#include <model/include/Map.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/RefineTransformation.h>
#include <model/include/Submodel.h>

#include <nodes/include/NeuralNetworkPredictorNode.h>

#include <utilities/include/CommandLineParser.h>
#include <utilities/include/Exception.h>
#include <utilities/include/OutputStreamImpostor.h>
#include <utilities/include/StringUtil.h>

#include <iostream>

using namespace ell;
using namespace ell::utilities;

bool IsNeuralNetworkPredictorNode(const ell::model::Node* node)
{
    if (dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<float>*>(node) != nullptr)
    {
        return true;
    }

    return dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<double>*>(node) != nullptr;
}

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedMapLoadArguments mapLoadArguments;
        ParsedPrintArguments printArguments;
        common::ParsedMapCompilerArguments mapCompilerArguments;
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(printArguments);
        commandLineParser.AddDocumentationString("");
        commandLineParser.AddDocumentationString("Compile options (only valid if 'compile' is true)");
        commandLineParser.AddOptionSet(mapCompilerArguments);
        commandLineParser.Parse();

        if (argc == 1)
        {
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 1;
        }

        // if no input specified, print help and exit
        if (!mapLoadArguments.HasInputFilename())
        {
            if (commandLineParser.GetPositionalArgs().size() == 1)
            {
                mapLoadArguments.inputMapFilename = commandLineParser.GetPositionalArgs()[0];
            }
            else
            {
                std::cout << commandLineParser.GetHelpString() << std::endl;
                return 1;
            }
        }

        // Load model from file
        model::Model model;
        if (mapLoadArguments.HasModelFilename())
        {
            // not all models can be turned into maps if they have no input or output nodes,
            // so we can't use LoadMap here.
            model = common::LoadModel(mapLoadArguments.inputModelFilename);
            if (printArguments.refine > 0)
            {
                model::TransformContext context;
                model::ModelTransformer transformer;
                model::RefineTransformation t;
                model = t.TransformModel(model, transformer, context);
            }
        }
        else // load a map
        {
            model::Map map = LoadMap(mapLoadArguments);
            if (printArguments.refine > 0)
            {
                map.Refine(printArguments.refine);
                model = map.GetModel().ShallowCopy();
            }
            else
            {
                model = map.GetModel().ShallowCopy();
            }

            if (printArguments.compile)
            {
                model::MapCompilerOptions settings = mapCompilerArguments.GetMapCompilerOptions("model");
                auto optimizerOptions = mapCompilerArguments.GetModelOptimizerOptions();

                model::IRMapCompiler compiler(settings, optimizerOptions);
                auto compiledMap = compiler.Compile(map);
                model = compiledMap.GetModel().ShallowCopy();
            }
        }

        // print model
        utilities::OutputStreamImpostor out = printArguments.outputStream;
        auto lowerOutputFormat = ToLowercase(printArguments.outputFormat);
        if (lowerOutputFormat == "dgml" || lowerOutputFormat == "dot")
        {
            PrintGraph(model, lowerOutputFormat, out, printArguments.includeNodeId);
        }
        else
        {
            PrintModel(model, out, { printArguments.includeNodeId, printArguments.nodeDetails });
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
    catch (utilities::LogicException& exception)
    {
        std::cerr << "runtime error: " << exception.GetMessage() << std::endl;
        return 1;
    }
    catch (utilities::InputException& exception)
    {
        std::cerr << "input error: " << exception.GetMessage() << std::endl;
        return 1;
    }
    catch (std::exception& exception)
    {
        std::cerr << "unknown error: " << exception.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
