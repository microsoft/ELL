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
#include "MapCompilerArguments.h"
#include "MapLoadArguments.h"

// model
#include "Map.h"
#include "IRCompiledMap.h"
#include "IRMapCompiler.h"
#include "OutputNode.h"

// passes
#include "StandardPasses.h"

// stl
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

void ProduceMapOutput(ParsedCompileArguments& compileArguments, common::ParsedMapCompilerArguments& mapCompilerArguments, common::MapLoadArguments& mapLoadArguments, model::Map& map)
{
    std::stringstream timingOutput;

    auto inputFilename = mapLoadArguments.GetInputFilename();
    auto outputDirectory = compileArguments.outputDirectory;

    auto baseFilename = compileArguments.outputFilenameBase;
    if (baseFilename.empty())
    {
        baseFilename = utilities::RemoveFileExtension(inputFilename);
    }
    if (!outputDirectory.empty())
    {
        ell::utilities::EnsureDirectoryExists(outputDirectory);
        baseFilename = utilities::JoinPaths(outputDirectory, utilities::GetFileName(baseFilename));
    }

    model::MapCompilerOptions settings = mapCompilerArguments.GetMapCompilerOptions(baseFilename);
    if (compileArguments.outputRefinedMap)
    {
        model::TransformContext context;
        TimingOutputCollector timer(timingOutput, "Time to refine map", compileArguments.verbose);
        map.Refine(context, compileArguments.maxRefinementIterations);
        timer.Stop();
        common::SaveMap(map, baseFilename + "_refined.map");
    }

    model::IRMapCompiler compiler(settings);
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
        if (compileArguments.outputAssembly)
        {
            TimingOutputCollector timer(timingOutput, "Time to save assembly code", compileArguments.verbose);
            compiledMap.WriteCode(baseFilename + ".s", emitters::ModuleOutputFormat::assembly);
        }
        if (compileArguments.outputObjectCode)
        {
            TimingOutputCollector timer(timingOutput, "Time to save object code", compileArguments.verbose);
            compiledMap.WriteCode(baseFilename + ".o", emitters::ModuleOutputFormat::objectCode);
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
        common::ParsedMapCompilerArguments mapCompilerArguments;

        commandLineParser.AddDocumentationString("Input file options");
        commandLineParser.AddOptionSet(mapLoadArguments);

        commandLineParser.AddDocumentationString("");
        commandLineParser.AddOptionSet(compileArguments);

        commandLineParser.AddDocumentationString("Code generation options");
        commandLineParser.AddOptionSet(mapCompilerArguments);

        // parse command line
        commandLineParser.Parse();

        // if no input specified, print help and exit
        if (!mapLoadArguments.HasInputFilename())
        {
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 0;
        }

        // Initialize the pass registry
        passes::AddStandardPassesToRegistry();

        // load map and produce the desired output
        TimingOutputCollector timer(timingOutput, "Time to load map", compileArguments.verbose);
        auto map = common::LoadMap(mapLoadArguments);
        timer.Stop();
        ProduceMapOutput(compileArguments, mapCompilerArguments, mapLoadArguments, map);

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
