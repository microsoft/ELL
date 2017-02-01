////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (compile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompileArguments.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

// dataset
#include "Dataset.h"
#include "Example.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "DataSaveArguments.h"
#include "LoadModel.h"
#include "MapLoadArguments.h"

// model
#include "DynamicMap.h"
#include "IRCompiledMap.h"
#include "IRMapCompiler.h"
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>

using namespace ell;

typedef std::function<void(const double*, double*)> FnInputOutput;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedMapLoadArguments mapLoadArguments;
        ParsedCompileArguments compileArguments;

        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(compileArguments);

        // parse command line
        commandLineParser.Parse();

        // load map
        auto map = common::LoadMap(mapLoadArguments);

        // create the desired output type
        if (CompileArguments::OutputType::refinedMap == compileArguments.outputType)
        {
            model::TransformContext context;
            map.Refine(context, compileArguments.maxRefinementIterations);
            common::SaveMap(map, compileArguments.outputCodeStream, "json");
        }
        else
        {
            model::IRCompiledMap compiledMap{ std::move(map), compileArguments.compiledFunctionName, compileArguments.optimize };
            switch (compileArguments.outputType)
            {
                case CompileArguments::OutputType::compiledMap:
                    common::SaveMap(compiledMap, compileArguments.outputCodeStream, "json");
                    break;

                case CompileArguments::OutputType::ir:
                    compiledMap.WriteCode(compileArguments.outputCodeStream, emitters::ModuleOutputFormat::ir);
                    break;

                case CompileArguments::OutputType::bitcode:
                    compiledMap.WriteCode(compileArguments.outputCodeStream, emitters::ModuleOutputFormat::bitcode);
                    break;

                case CompileArguments::OutputType::assembly:
                    {
                        emitters::MachineCodeOutputOptions compileAssemblyOptions;
                        compileAssemblyOptions.optimizationLevel = emitters::OptimizationLevel::Default;
                        compileAssemblyOptions.cpu = compileArguments.cpu;
                        if ("cortex-m4" == compileArguments.cpu)
                        {
                            compileAssemblyOptions.triple = "arm-none-eabi";
                            compileAssemblyOptions.targetFeatures = "+armv7e-m,+v7,soft-float";
                        }

                        compiledMap.WriteCode(compileArguments.outputCodeStream, emitters::ModuleOutputFormat::assembly, compileAssemblyOptions);
                    }
                
                default:
                    throw emitters::EmitterException(emitters::EmitterError::notSupported);
            }
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
        std::cerr << "exception: " << exception.GetMessage() << std::endl;
        return 1;
    }

    // the end
    return 0;
}
