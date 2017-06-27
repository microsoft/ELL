////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (makeExamples)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GenerateModels.h"
#include "ModelGenerateArguments.h"

// common
#include "LoadModel.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <iostream>

using namespace ell;

void SaveModels(const std::string& ext)
{
    common::SaveModel(GenerateIdentityModel(3), "identity." + ext);
    common::SaveModel(GenerateTimesTwoModel(3), "times_two." + ext);
    common::SaveModel(GenerateIsEqualModel(), "is_equal." + ext);
    common::SaveModel(GenerateArgMaxModel(3), "arg_max." + ext);

    common::SaveModel(GenerateModel1(), "model_1." + ext);
    common::SaveModel(GenerateModel2(), "model_2." + ext);
    common::SaveModel(GenerateModel3(), "model_3." + ext);

    common::SaveModel(GenerateTreeModel(0), "tree_0." + ext);
    common::SaveModel(GenerateTreeModel(1), "tree_1." + ext);
    common::SaveModel(GenerateTreeModel(2), "tree_2." + ext);
    common::SaveModel(GenerateTreeModel(3), "tree_3." + ext);

    common::SaveModel(GenerateRefinedTreeModel(0), "refined_tree_0." + ext);
    common::SaveModel(GenerateRefinedTreeModel(1), "refined_tree_1." + ext);
    common::SaveModel(GenerateRefinedTreeModel(2), "refined_tree_2." + ext);
    common::SaveModel(GenerateRefinedTreeModel(3), "refined_tree_3." + ext);

    common::SaveModel(GenerateMultiOutModel(3), "multi_out." + ext);
}

void SaveMaps(const std::string& ext)
{
    common::SaveMap(GenerateSteppableMap(10, 50), "ELL_step10." + ext);
}

int main(int argc, char* argv[])
{
    try
    {
        ParsedModelGenerateArguments arguments;

        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        commandLineParser.AddOptionSet(arguments);

        // parse command line
        commandLineParser.Parse();

        if (ModelGenerateArguments::OutputType::map == arguments.outputType)
        {
            SaveMaps("map");
        }
        else
        {
            // Default to generating models
            SaveModels("model");
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
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        throw;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ERROR, got std exception. Message: " << exception.what() << std::endl;
        throw;
    }
    catch (...)
    {
        std::cerr << "ERROR, got unknown exception." << std::endl;
        throw;
    }
}