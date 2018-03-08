////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProfileArguments.h"

// tools/PythonPlugin
#include "InvokePython.h"

// common
#include "LoadModel.h"
#include "MapCompilerArguments.h"
#include "ModelLoadArguments.h"

// math
#include "Tensor.h"

// model
#include "Map.h"
#include "IRCompiledMap.h"
#include "IRMapCompiler.h"

// passes
#include "StandardPasses.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "MillisecondTimer.h"
#include "OutputStreamImpostor.h"
#include "RandomEngines.h"
#include "TypeName.h"
#include "Unused.h"

// stl
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace ell;

template <typename InputType, utilities::IsIntegral<InputType> = true>
std::vector<InputType> GetInputVector(const math::TensorShape& inputShape)
{
    auto inputSize = inputShape.Size();
    std::vector<InputType> result(inputSize);
    auto engine = utilities::GetRandomEngine("123");
    std::uniform_int_distribution<InputType> dist(0, 255);
    for (size_t index = 0; index < inputSize; ++index)
    {
        result[index] = dist(engine);
    }
    return result;
}

template <typename InputType, utilities::IsFloatingPoint<InputType> = true>
std::vector<InputType> GetInputVector(const math::TensorShape& inputShape)
{
    auto inputSize = inputShape.Size();
    std::vector<InputType> result(inputSize);
    auto engine = utilities::GetRandomEngine("123");
    std::uniform_real_distribution<InputType> dist(0, std::nextafter(255, std::numeric_limits<InputType>::max()));
    for (size_t index = 0; index < inputSize; ++index)
    {
        result[index] = dist(engine);
    }
    return result;
}

//
// Test-data-related
//
template <typename InputType>
std::vector<InputType> GetInputConverted(std::string filename, const std::vector<std::string>& converterArgs)
{
    auto result = ExecutePythonScript(filename, converterArgs);
    return std::vector<InputType>(result.begin(), result.end());
}

template <typename InputType>
std::vector<InputType> GetModelInput(model::Map& map, const ProfileArguments& profileArguments, const std::vector<std::string>& converterArgs)
{
    math::TensorShape inputShape = map.GetInputShape();
    if (profileArguments.inputConverter.empty())
    {
        return GetInputVector<InputType>(inputShape);
    }
    else
    {
        return GetInputConverted<InputType>(profileArguments.inputConverter, converterArgs);
    }
}

//
// Output-related
//
utilities::OutputStreamImpostor GetOutputStream(const std::string& filename)
{
    if (filename == "" || filename == "<null>")
    {
        return { utilities::OutputStreamImpostor::StreamType::null };
    }
    if (filename == "<cout>")
    {
        return { utilities::OutputStreamImpostor::StreamType::cout };
    }
    return { filename };
}

void WriteUserComment(const std::string& comment, ProfileOutputFormat format,std::ostream& out)
{
    if (format == ProfileOutputFormat::text)
    {
        out << "Comment: " << comment << "\n";
    }
    else // json
    {
        out << "\"comment\": \"" << comment << "\"\n";
    }
}

void WriteModelStatistics(model::IRCompiledMap& map, ProfileOutputFormat format, std::ostream& out)
{
    // get overall stats
    auto modelStats = map.GetModelPerformanceCounters();

    if (format == ProfileOutputFormat::text)
    {
        std::ios::fmtflags savedFlags(out.flags());
        out << std::fixed;
        out.precision(5);

        int count = modelStats->count;
        double totalTime = modelStats->totalTime;
        double timePerRun = totalTime / count;

        out << "\nModel statistics" << std::endl;
        out << "Total time: " << totalTime << " ms \tcount: " << count << "\t time per run: " << timePerRun << " ms" << std::endl;

        out.flags(savedFlags);
    }
    else // json
    {
        int count = modelStats->count;
        double totalTime = modelStats->totalTime;
        double timePerRun = totalTime / count;

        out << "\"model_statistics\": {\n";
        out << "  \"total_time\": " << totalTime << ",\n";
        out << "  \"average_time\": " << timePerRun << ",\n";
        out << "  \"count\": " << count << "\n";
        out << "}";
    }
}

void WriteNodeStatistics(model::IRCompiledMap& map, ProfileOutputFormat format, std::ostream& out)
{
    // Gather node statistics
    std::vector<std::pair<model::NodeInfo, model::PerformanceCounters>> nodeInfo;
    auto numNodes = map.GetNumProfiledNodes();
    for (int index = 0; index < numNodes; ++index)
    {
        auto info = map.GetNodeInfo(index);
        auto stats = map.GetNodePerformanceCounters(index);
        nodeInfo.emplace_back(*info, *stats);
    }

    size_t maxTypeLength = 0;
    std::vector<std::pair<model::NodeInfo, model::PerformanceCounters>> nodeTypeInfo;
    auto numNodeTypes = map.GetNumProfiledNodeTypes();
    for (int index = 0; index < numNodeTypes; ++index)
    {
        auto info = map.GetNodeTypeInfo(index);
        auto stats = map.GetNodeTypePerformanceCounters(index);
        nodeTypeInfo.emplace_back(*info, *stats);
        maxTypeLength = std::max(maxTypeLength, std::strlen(info->nodeType));
    }
    std::sort(nodeTypeInfo.begin(), nodeTypeInfo.end(), [](auto a, auto b) { return a.second.totalTime < b.second.totalTime; });

    // Write node statistics
    if (format == ProfileOutputFormat::text)
    {
        std::ios::fmtflags savedFlags(out.flags());
        out << std::fixed;
        out.precision(5);

        out << "Node statistics" << std::endl;
        for (const auto& info : nodeInfo)
        {
            out << "Node[" << info.first.nodeName << "]:\t" << std::setw(maxTypeLength) << std::left << info.first.nodeType << "\ttime: " << info.second.totalTime << " ms\tcount: " << info.second.count << "\n";
        }

        out << "\n\n";
        out << "Node type statistics" << std::endl;
        for (const auto& info : nodeTypeInfo)
        {
            out << std::setw(maxTypeLength) << std::left << info.first.nodeType << "\ttime: " << info.second.totalTime << " ms \tcount: " << info.second.count << "\n";
        }

        out.flags(savedFlags);
    }
    else // json
    {
        out << "\"node_statistics\": [\n";
        for (const auto& info : nodeInfo)
        {
            out << "  {\n";
            out << "    \"name\": "
                << "\"" << info.first.nodeName << "\",\n";
            out << "    \"type\": "
                << "\"" << info.first.nodeType << "\",\n";
            out << "    \"total_time\": " << info.second.totalTime << ",\n";
            out << "    \"average_time\": " << info.second.totalTime / info.second.count << ",\n";
            out << "    \"count\": " << info.second.count << "\n";
            out << "  }";
            bool isLast = (&info == &nodeInfo.back());
            if (!isLast)
            {
                out << ",";
            }
            out << "\n";
        }
        out << "],\n";

        out << "\"node_type_statistics\": [\n";
        out << "  [";
        for (const auto& info : nodeTypeInfo)
        {
            out << "  {\n";
            out << "    \"type\": "
                << "\"" << info.first.nodeType << "\",\n";
            out << "    \"total_time\": " << info.second.totalTime << ",\n";
            out << "    \"average_time\": " << info.second.totalTime / info.second.count << ",\n";
            out << "    \"count\": " << info.second.count << "\n";
            out << "  }";
            bool isLast = (&info == &nodeTypeInfo.back());
            if (!isLast)
            {
                out << ",";
            }
            out << "\n";
        }
        out << "]";
    }
}

void WriteTimingDetail(std::ostream& timingOutputStream, ProfileOutputFormat format, const std::vector<std::vector<double>>& nodeTimings)
{
    std::string beginArray = "";
    std::string endArray = "";
    std::string elementDelimiter = "\t";
    std::string arrayDelimiter = "\n";
    if (format == ProfileOutputFormat::json)
    {
        elementDelimiter = ", ";
        arrayDelimiter = ",\n";
        beginArray = "[";
        endArray = "]";
    }

    timingOutputStream << beginArray;
    auto numIterations = nodeTimings.size();
    for (size_t iter = 0; iter < numIterations; ++iter)
    {
        timingOutputStream << beginArray;
        auto numNodes = nodeTimings[iter].size();
        for (size_t nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex)
        {
            auto timing = (iter == 0) ? (nodeTimings[iter][nodeIndex]) : (nodeTimings[iter][nodeIndex] - nodeTimings[iter - 1][nodeIndex]);
            timingOutputStream << timing;
            if (nodeIndex < numNodes - 1)
            {
                timingOutputStream << elementDelimiter;
            }
        }
        timingOutputStream << endArray;
        if (iter < numIterations - 1)
        {
            timingOutputStream << arrayDelimiter;
        }
    }
    timingOutputStream << endArray;
}

//
// Profiling functions
//
void ResetProfilingInfo(model::IRCompiledMap& map)
{
    map.ResetModelProfilingInfo();
    map.ResetNodeProfilingInfo();
    map.ResetNodeTypeProfilingInfo();
}

template <typename InputType, typename OutputType>
void WarmUpModel(model::IRCompiledMap& map, const std::vector<InputType>& input, int numBurnInIterations)
{
    for (int iter = 0; iter < numBurnInIterations; ++iter)
    {
        auto output = map.Compute<OutputType>(input);
    }
    ResetProfilingInfo(map);
}

template <typename InputType, typename OutputType>
void TimeModel(model::Map& map, const std::vector<InputType>& input, const ProfileArguments& profileArguments, const common::MapCompilerArguments& mapCompilerArguments)
{
    // Get output stream
    auto outputStream = GetOutputStream(profileArguments.outputFilename);

    // Initialize pass registry
    passes::AddStandardPassesToRegistry();

    // Compile map
    model::MapCompilerOptions settings = mapCompilerArguments.GetMapCompilerOptions("");
    settings.profile = false;
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);

    std::cout << "Compiling model" << std::endl;
    auto compiledMap = compiler.Compile(map);

    // Warm up the system by evaluating the model some number of times
    WarmUpModel<InputType, OutputType>(compiledMap, input, profileArguments.numBurnInIterations);

    // Now evaluate the model and time it
    utilities::MillisecondTimer timer;
    for (int iter = 0; iter < profileArguments.numIterations; ++iter)
    {
        auto output = compiledMap.Compute<OutputType>(input);
    }
    float totalTime = static_cast<float>(timer.Elapsed());

    if (profileArguments.outputFormat == ProfileOutputFormat::text)
    {
        outputStream << "Num iterations: " << profileArguments.numIterations << std::endl;
        outputStream << "Total time: " << totalTime << " ms" << std::endl;
        outputStream << "Average time: " << totalTime / profileArguments.numIterations << " ms" << std::endl;
    }
    else // json
    {
        outputStream << "{\n";
        outputStream << "\"total_time\": " << totalTime << ",\n";
        outputStream << "\"average_time\": " << totalTime / profileArguments.numIterations << ",\n";
        outputStream << "\"count\": " << profileArguments.numIterations << "\n";
        outputStream << "}\n";
    }
}

template <typename InputType, typename OutputType>
void ProfileModel(model::Map& map, const ProfileArguments& profileArguments, const common::MapCompilerArguments& mapCompilerArguments, const std::vector<std::string>& converterArgs)
{
    const bool printTimingChart = profileArguments.timingOutputFilename != "";
    auto profileOutputStream = GetOutputStream(profileArguments.outputFilename);
    auto timingOutputStream = GetOutputStream(profileArguments.timingOutputFilename);
    const auto comment = profileArguments.outputComment;

    std::vector<InputType> input = GetModelInput<InputType>(map, profileArguments, converterArgs);

    // In "summary only" mode, we don't compile the model with profiling enabled
    // (because we just want the overall run time), so we have a separate codepath
    // for that option
    if (profileArguments.summaryOnly)
    {
        TimeModel<InputType, OutputType>(map, input, profileArguments, mapCompilerArguments);
        return;
    }

    // Initialize pass registry
    passes::AddStandardPassesToRegistry();

    // Compile map
    model::MapCompilerOptions settings = mapCompilerArguments.GetMapCompilerOptions("");
    settings.profile = true;
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);

    std::cout << "Compiling model" << std::endl;
    auto compiledMap = compiler.Compile(map);

    auto numNodes = compiledMap.GetNumProfiledNodes();
    std::vector<std::vector<double>> nodeTimings(profileArguments.numIterations); // per-node timing
    for (auto& vec : nodeTimings)
    {
        vec.resize(numNodes);
    }

    // Warm up the system by evaluating the model some number of times
    WarmUpModel<InputType, OutputType>(compiledMap, input, profileArguments.numBurnInIterations);

    // Now evaluate the model and record the profiling info
    for (int iter = 0; iter < profileArguments.numIterations; ++iter)
    {
        // Exercise the model
        auto output = compiledMap.Compute<OutputType>(input);

        if (printTimingChart)
        {
            for (int nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex)
            {
                auto stats = compiledMap.GetNodePerformanceCounters(nodeIndex);
                auto time = stats->totalTime;
                nodeTimings[iter][nodeIndex] = time;
            }
        }
    }

    auto format = profileArguments.outputFormat;
    if (printTimingChart)
    {
        WriteTimingDetail(timingOutputStream, format, nodeTimings);
    }

    // print profile info
    if (format == ProfileOutputFormat::text)
    {
        if (!comment.empty())
        {
            WriteUserComment(comment, format, profileOutputStream);
        }
        WriteNodeStatistics(compiledMap, format, profileOutputStream);
        WriteModelStatistics(compiledMap, format, profileOutputStream);
    }
    else
    {
        profileOutputStream << "{\n";
        if (!comment.empty())
        {
            WriteUserComment(comment, format, profileOutputStream);
            profileOutputStream << ",\n";
        }
        WriteNodeStatistics(compiledMap, format, profileOutputStream);
        profileOutputStream << ",\n";
        WriteModelStatistics(compiledMap, format, profileOutputStream);
        profileOutputStream << "}\n";
    }
}

template <typename InputType>
void ProfileModel(model::Map& map, const ProfileArguments& profileArguments, const common::MapCompilerArguments& mapCompilerArguments, const std::vector<std::string>& converterArgs)
{
    switch (map.GetOutputType())
    {
        case model::Port::PortType::smallReal:
            ProfileModel<InputType, model::ValueType<model::Port::PortType::smallReal>>(map, profileArguments, mapCompilerArguments, converterArgs);
            break;
        case model::Port::PortType::real:
            ProfileModel<InputType, model::ValueType<model::Port::PortType::real>>(map, profileArguments, mapCompilerArguments, converterArgs);
            break;
        case model::Port::PortType::integer:
            ProfileModel<InputType, model::ValueType<model::Port::PortType::integer>>(map, profileArguments, mapCompilerArguments, converterArgs);
            break;
        case model::Port::PortType::bigInt:
            ProfileModel<InputType, model::ValueType<model::Port::PortType::bigInt>>(map, profileArguments, mapCompilerArguments, converterArgs);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Model has an unsupported output type");
    }
}

//
// Load the map and process it
//
void ProfileModel(model::Map& map, const ProfileArguments& profileArguments, const common::MapCompilerArguments& mapCompilerArguments, const std::vector<std::string>& converterArgs)
{
    switch (map.GetInputType())
    {
        case model::Port::PortType::smallReal:
            ProfileModel<model::ValueType<model::Port::PortType::smallReal>>(map, profileArguments, mapCompilerArguments, converterArgs);
            break;
        case model::Port::PortType::real:
            ProfileModel<model::ValueType<model::Port::PortType::real>>(map, profileArguments, mapCompilerArguments, converterArgs);
            break;
        case model::Port::PortType::integer:
            ProfileModel<model::ValueType<model::Port::PortType::integer>>(map, profileArguments, mapCompilerArguments, converterArgs);
            break;
        case model::Port::PortType::bigInt:
            ProfileModel<model::ValueType<model::Port::PortType::bigInt>>(map, profileArguments, mapCompilerArguments, converterArgs);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Model has an unsupported input type");
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
        commandLineParser.AddOptionSet(mapLoadArguments);
        ParsedProfileArguments profileArguments;
        commandLineParser.AddOptionSet(profileArguments);
        common::ParsedMapCompilerArguments compileArguments;
        commandLineParser.AddDocumentationString("Code generation options");
        commandLineParser.AddOptionSet(compileArguments);
        commandLineParser.DisableOption("--profile");
        commandLineParser.Parse();

        // if no input specified, print help and exit
        if (!mapLoadArguments.HasInputFilename())
        {
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 0;
        }

        // load map file
        auto map = common::LoadMap(mapLoadArguments);
        ProfileModel(map, profileArguments, compileArguments, commandLineParser.GetPassthroughArgs());
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
