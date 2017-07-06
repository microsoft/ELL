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
#include "MillisecondTimer.h"

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
#include <sstream>
#include <stdexcept>
#include <string>

using namespace ell;

typedef std::function<void(const double*, double*)> FnInputOutput;

class TimingOutputCollector
{
public:
    TimingOutputCollector(std::ostream& stream, const std::string& message, bool enabled)
        : _valid(true), _enabled(enabled), _stream(stream), _message(message) {}

    ~TimingOutputCollector()
    {
        ReportTime();
    }

    void Start()
    {
        _timer.Start();
        _valid = true;
    }

    void Start(const std::string& newMessage)
    {
        _message = newMessage;
        _timer.Start();
        _valid = true;
    }

    void Stop()
    {
        ReportTime();
        _valid = false;
    }

private:
    bool _valid;
    bool _enabled;
    utilities::MillisecondTimer _timer;
    std::ostream& _stream;
    std::string _message;

    void ReportTime()
    {
        if (_valid && _enabled)
        {
            auto elapsed = _timer.Elapsed();
            _stream << _message << ": " << elapsed << " ms\n";
        }
    }
};

template <typename MapType, typename MapCompilerType>
void ProduceMapOutput(ParsedCompileArguments& compileArguments, common::MapLoadArguments& mapLoadArguments, MapType& map)
{
    std::stringstream timingOutput;

    // create the desired output type(s)
    bool namespaceSpecified = true;
    auto namespacePrefix = compileArguments.compiledModuleName;
    if (namespacePrefix == "")
    {
        namespacePrefix = "ELL";
        namespaceSpecified = false;
    }

    auto inputFilename = mapLoadArguments.GetInputFilename();
    auto outputDirectory = compileArguments.outputDirectory;
    auto baseFilename = utilities::RemoveFileExtension(inputFilename);
    if(outputDirectory != "")
    {
        baseFilename = utilities::JoinPaths(outputDirectory, utilities::GetFileName(baseFilename));
    }

    std::string functionName;
    if (compileArguments.compiledFunctionName == "")
    {
        if (namespaceSpecified)
        {
            functionName = namespacePrefix + "_predict";
        }
        else
        {
            functionName = baseFilename;
        }
    }
    else
    {
        functionName = namespacePrefix + "_" + compileArguments.compiledFunctionName;
    }

    model::MapCompilerParameters settings;
    settings.moduleName = namespacePrefix;
    settings.mapFunctionName = functionName;
    settings.compilerSettings.useBlas = compileArguments.useBlas;
    settings.compilerSettings.optimize = compileArguments.optimize;
    settings.profile = compileArguments.profile;

    if (compileArguments.target != "")
    {
        settings.compilerSettings.targetDevice.deviceName = compileArguments.target;
    }

    if (compileArguments.targetTriple != "")
    {
        settings.compilerSettings.targetDevice.triple = compileArguments.targetTriple;
    }

    if (compileArguments.targetDataLayout != "")
    {
        settings.compilerSettings.targetDevice.dataLayout = compileArguments.targetDataLayout;
    }

    if (compileArguments.targetFeatures != "")
    {
        settings.compilerSettings.targetDevice.features = compileArguments.targetFeatures;
    }

    if (compileArguments.numBits != 0)
    {
        settings.compilerSettings.targetDevice.numBits = compileArguments.numBits;
    }

    if (compileArguments.outputRefinedMap)
    {
        model::TransformContext context;
        TimingOutputCollector timer(timingOutput, "Time to refine map", compileArguments.verbose);
        map.Refine(context, compileArguments.maxRefinementIterations);
        timer.Stop();
        common::SaveMap(map, baseFilename + "_refined.map");
    }

    MapCompilerType compiler(settings);
    TimingOutputCollector timer(timingOutput, "Time to compile map", compileArguments.verbose);
    auto compiledMap = compiler.Compile(map);
    timer.Stop();

    if (compileArguments.outputCompiledMap)
    {
        TimingOutputCollector timer(timingOutput, "Time to save compiled map", compileArguments.verbose);
        common::SaveMap(compiledMap, baseFilename + "_compiled.map");
    }
    if (compileArguments.outputHeader)
    {
        TimingOutputCollector timer(timingOutput, "Time to save header file", compileArguments.verbose);
        compiledMap.WriteCodeHeader(baseFilename + ".h");
    }
    if (compileArguments.outputIr)
    {
        TimingOutputCollector timer(timingOutput, "Time to save LLVM IR", compileArguments.verbose);
        compiledMap.WriteCode(baseFilename + ".ll", emitters::ModuleOutputFormat::ir);
    }
    if (compileArguments.outputBitcode)
    {
        TimingOutputCollector timer(timingOutput, "Time to save LLVM bitcode", compileArguments.verbose);
        compiledMap.WriteCode(baseFilename + ".bc", emitters::ModuleOutputFormat::bitcode);
    }
    if (compileArguments.outputAssembly || compileArguments.outputObjectCode)
    {
        emitters::MachineCodeOutputOptions compileMachineCodeOptions;

        if (compileArguments.outputAssembly)
        {
            TimingOutputCollector timer(timingOutput, "Time to save assembly code", compileArguments.verbose);
            compiledMap.WriteCode(baseFilename + ".s", emitters::ModuleOutputFormat::assembly, compileMachineCodeOptions);
        }
        if (compileArguments.outputObjectCode)
        {
            TimingOutputCollector timer(timingOutput, "Time to save object code", compileArguments.verbose);
            compiledMap.WriteCode(baseFilename + ".o", emitters::ModuleOutputFormat::objectCode, compileMachineCodeOptions);
        }
    }
    if (compileArguments.outputSwigInterface)
    {
        TimingOutputCollector timer(timingOutput, "Time to save SWIG interface", compileArguments.verbose);
        compiledMap.WriteCode(baseFilename + ".i", emitters::ModuleOutputFormat::swigInterface);
    }

    if (compileArguments.verbose)
    {
        std::cout << timingOutput.str();
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::stringstream timingOutput;

        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedMapLoadArguments mapLoadArguments;
        ParsedCompileArguments compileArguments;

        commandLineParser.AddDocumentationString("Input file options");
        commandLineParser.AddOptionSet(mapLoadArguments);

        commandLineParser.AddDocumentationString("");
        commandLineParser.AddOptionSet(compileArguments);

        // parse command line
        commandLineParser.Parse();

        // if no input specified, print help and exit
        if (!mapLoadArguments.HasInputFilename())
        {
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 0;
        }

        // load map and produce the desired output
        switch (mapLoadArguments.mapType)
        {
            // This ugliness should go away once we move to clock nodes (and abstract the clock type from map)
            case common::MapLoadArguments::MapType::steadyClockSteppableMap:
            {
                using MapType = model::SteppableMap<std::chrono::steady_clock>;
                using MapCompilerType = model::IRSteppableMapCompiler<std::chrono::steady_clock>;
                constexpr auto MapArgumentType = common::MapLoadArguments::MapType::steadyClockSteppableMap;

                TimingOutputCollector timer(timingOutput, "Time to load map", compileArguments.verbose);
                auto map = common::LoadMap<MapType, MapArgumentType>(mapLoadArguments);
                timer.Stop();
                ProduceMapOutput<MapType, MapCompilerType>(compileArguments, mapLoadArguments, map);
                break;
            }

            case common::MapLoadArguments::MapType::systemClockSteppableMap:
            {
                using MapType = model::SteppableMap<std::chrono::system_clock>;
                using MapCompilerType = model::IRSteppableMapCompiler<std::chrono::system_clock>;
                constexpr auto MapArgumentType = common::MapLoadArguments::MapType::systemClockSteppableMap;

                TimingOutputCollector timer(timingOutput, "Time to load map", compileArguments.verbose);
                auto map = common::LoadMap<MapType, MapArgumentType>(mapLoadArguments);
                timer.Stop();
                ProduceMapOutput<MapType, MapCompilerType>(compileArguments, mapLoadArguments, map);
                break;
            }

            case common::MapLoadArguments::MapType::simpleMap:
            {
                using MapType = model::DynamicMap;
                using MapCompilerType = model::IRMapCompiler;

                TimingOutputCollector timer(timingOutput, "Time to load map", compileArguments.verbose);
                auto map = common::LoadMap(mapLoadArguments);
                timer.Stop();
                ProduceMapOutput<MapType, MapCompilerType>(compileArguments, mapLoadArguments, map);
                break;
            }

            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: couldn't read input file.");
        }

        if (compileArguments.verbose)
        {
            std::cout << timingOutput.str();
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
