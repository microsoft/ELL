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

// dataset
#include "Dataset.h"

// common
#include "LoadModel.h"
#include "MapLoadArguments.h"

// model
#include "DynamicMap.h"
#include "IRCompiledMap.h"
#include "IRMapCompiler.h"
#include "IRSteppableMapCompiler.h"
#include "OutputNode.h"

// stl
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace ell;

typedef std::function<void(const double*, double*)> FnInputOutput;

template <typename MapType, typename MapCompilerType>
void ProduceMapOutput(const common::MapLoadArguments& mapLoadArguments, ParsedCompileArguments& compileArguments, MapType& map)
{
    // create the desired output type
    if (CompileArguments::OutputType::refinedMap == compileArguments.outputType)
    {
        model::TransformContext context;
        map.Refine(context, compileArguments.maxRefinementIterations);
        common::SaveMap(map, compileArguments.outputCodeStream);
    }
    else
    {
        model::MapCompilerParameters settings;
        settings.mapFunctionName = compileArguments.compiledFunctionName;
        settings.moduleName = compileArguments.compiledModuleName;
        settings.compilerSettings.optimize = compileArguments.optimize;

        MapCompilerType compiler(settings);
        auto compiledMap = compiler.Compile(map);

        switch (compileArguments.outputType)
        {
            case CompileArguments::OutputType::compiledMap:
                common::SaveMap(compiledMap, compileArguments.outputCodeStream);
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
                compileAssemblyOptions.targetDevice.cpu = compileArguments.cpu;
                if ("cortex-m4" == compileArguments.cpu)
                {
                    compileAssemblyOptions.targetDevice.triple = "arm-none-eabi";
                    compileAssemblyOptions.targetDevice.features = "+armv7e-m,+v7,soft-float";
                }

                compiledMap.WriteCode(compileArguments.outputCodeStream, emitters::ModuleOutputFormat::assembly, compileAssemblyOptions);
            }

            case CompileArguments::OutputType::swigInterface:
                compiledMap.WriteCode(compileArguments.outputFilename, emitters::ModuleOutputFormat::swigInterface);
                break;

            default:
                throw emitters::EmitterException(emitters::EmitterError::notSupported);
        }
    }
}

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

        // load map and produce the desired output
        switch (mapLoadArguments.mapType)
        {
            // This ugliness should go away once we move to clock nodes (and abstract the clock type from map)
            case common::MapLoadArguments::MapType::steadyClockSteppableMap:
            {
                using MapType = model::SteppableMap<std::chrono::steady_clock>;
                using MapCompilerType = model::IRSteppableMapCompiler<std::chrono::steady_clock>;
                constexpr auto MapArgumentType = common::MapLoadArguments::MapType::steadyClockSteppableMap;

                auto map = common::LoadMap<MapType, MapArgumentType>(mapLoadArguments);
                ProduceMapOutput<MapType, MapCompilerType>(mapLoadArguments, compileArguments, map);
                break;
            }

            case common::MapLoadArguments::MapType::systemClockSteppableMap:
            {
                using MapType = model::SteppableMap<std::chrono::system_clock>;
                using MapCompilerType = model::IRSteppableMapCompiler<std::chrono::system_clock>;
                constexpr auto MapArgumentType = common::MapLoadArguments::MapType::systemClockSteppableMap;

                auto map = common::LoadMap<MapType, MapArgumentType>(mapLoadArguments);
                ProduceMapOutput<MapType, MapCompilerType>(mapLoadArguments, compileArguments, map);
                break;
            }

            case common::MapLoadArguments::MapType::simpleMap:
            {
                using MapType = model::DynamicMap;
                using MapCompilerType = model::IRMapCompiler;

                auto map = common::LoadMap(mapLoadArguments);
                ProduceMapOutput<MapType, MapCompilerType>(mapLoadArguments, compileArguments, map);
                break;
            }

            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: couldn't read file.");
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
