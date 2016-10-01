////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (makeExamples)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GenerateModels.h"

// common
#include "LoadModel.h"

// model
#include "Model.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <iostream>

namespace emll
{
void SaveModels(std::string ext)
{
    auto model1 = GenerateModel1();
    auto model2 = GenerateModel2();
    auto model3 = GenerateModel3();
    auto tree0 = GenerateTreeModel(0);
    auto tree1 = GenerateTreeModel(1);
    auto tree2 = GenerateTreeModel(2);
    auto tree3 = GenerateTreeModel(3);
    auto refinedTree0 = GenerateRefinedTreeModel(0);
    auto refinedTree1 = GenerateRefinedTreeModel(1);
    auto refinedTree2 = GenerateRefinedTreeModel(2);
    auto refinedTree3 = GenerateRefinedTreeModel(3);

    common::SaveModel(model1, "model_1." + ext);
    common::SaveModel(model2, "model_2." + ext);
    common::SaveModel(model3, "model_3." + ext);

    common::SaveModel(tree0, "tree_0." + ext);
    common::SaveModel(tree1, "tree_1." + ext);
    common::SaveModel(tree2, "tree_2." + ext);
    common::SaveModel(tree3, "tree_3." + ext);

    common::SaveModel(refinedTree0, "refined_tree_0." + ext);
    common::SaveModel(refinedTree1, "refined_tree_1." + ext);
    common::SaveModel(refinedTree2, "refined_tree_2." + ext);
    common::SaveModel(refinedTree3, "refined_tree_3." + ext);
}
}

using namespace emll;

int main(int argc, char* argv[])
{
    try
    {
        std::string format = "json";

        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);
        commandLineParser.AddOption(format, "modelFormat", "mf", "xml or json", "json");

        // parse command line
        commandLineParser.Parse();
        if (format == "both")
        {
            SaveModels("xml");
            SaveModels("json");
        }
        else
        {
            SaveModels(format);
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
        std::cerr << "ERROR, got EMLL exception. Message: " << exception.GetMessage() << std::endl;
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