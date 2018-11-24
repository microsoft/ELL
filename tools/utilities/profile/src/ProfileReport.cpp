////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProfileReport.cpp (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProfileReport.h"

// stl
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

// Characters that must be escaped in JSON strings: ', ", \, newline (\n), carriage return (\r), tab (\t), backspace (\b), form feed (\f)
std::string EncodeJSONString(const std::string& str)
{
    std::vector<char> charCodes(127, '\0');
    charCodes['\''] = '\'';
    charCodes['\"'] = '\"';
    charCodes['\\'] = '\\';
    charCodes['/'] = '/';
    charCodes['\n'] = 'n';
    charCodes['\r'] = 'r';
    charCodes['\t'] = 't';
    charCodes['\b'] = 'b';
    charCodes['\f'] = 'f';

    // copy characters from str until we hit an escaped character, then prepend it with a backslash
    std::stringstream s;
    for (auto ch : str)
    {
        auto encoding = ch >= 127 ? '\0' : charCodes[ch];
        if (encoding == '\0') // no encoding
        {
            s.put(ch);
        }
        else
        {
            s.put('\\');
            s.put(encoding);
        }
    }
    return s.str();
}

void WriteUserComment(const std::string& comment, ProfileOutputFormat format, std::ostream& out)
{
    if (format == ProfileOutputFormat::text)
    {
        out << "Comment: " << comment << "\n";
    }
    else // json
    {
        out << "\"comment\": \"" << EncodeJSONString(comment) << "\"\n";
    }
}

void WriteModelStatistics(const ELL_PerformanceCounters* modelStats, ProfileOutputFormat format, std::ostream& out)
{
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

void WriteNodeStatistics(std::vector<std::pair<ELL_NodeInfo, ELL_PerformanceCounters>>& nodeInfo, std::vector<std::pair<ELL_NodeInfo, ELL_PerformanceCounters>>& nodeTypeInfo, ProfileOutputFormat format, std::ostream& out)
{
    // Write node statistics
    if (format == ProfileOutputFormat::text)
    {
        std::ios::fmtflags savedFlags(out.flags());
        out << std::fixed;
        out.precision(5);

        // Find maximum length type name
        size_t maxTypeLength = 0;
        for (const auto& info : nodeTypeInfo)
        {
            maxTypeLength = std::max(maxTypeLength, std::strlen((const char*)(info.first.nodeType)));
        }

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
                << "\"" << EncodeJSONString((const char*)(info.first.nodeName)) << "\",\n";
            out << "    \"type\": "
                << "\"" << EncodeJSONString((const char*)(info.first.nodeType)) << "\",\n";
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
                << "\"" << EncodeJSONString((const char*)(info.first.nodeType)) << "\",\n";
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

void WriteRegionStatistics(std::vector<ELL_ProfileRegionInfo>& regions, ProfileOutputFormat format, std::ostream& out)
{
    // Write region statistics
    auto numRegions = regions.size();
    if (format == ProfileOutputFormat::text)
    {
        if (numRegions > 0)
        {
            std::ios::fmtflags savedFlags(out.flags());
            out << std::fixed;
            out.precision(5);

            size_t maxNameLength = 0;
            for (const auto& info : regions)
            {
                maxNameLength = std::max(maxNameLength, std::strlen((const char*)(info.name)));
            }

            out << "\nRegion statistics" << std::endl;
            for (const auto& info : regions)
            {
                out << "Region[" << info.name << "]:\t" << std::setw(maxNameLength) << std::left << "\ttime: " << info.totalTime << " ms\tcount: " << info.count << "\n";
            }

            out << "\n\n";
            out.flags(savedFlags);
        }
    }
    else // json
    {
        out << "\"region_statistics\": [\n";
        for (const auto& info : regions)
        {
            out << "  {\n";
            out << "    \"name\": "
                << "\"" << EncodeJSONString((const char*)(info.name)) << "\",\n";
            out << "    \"total_time\": " << info.totalTime << ",\n";
            out << "    \"average_time\": " << info.totalTime / info.count << ",\n";
            out << "    \"count\": " << info.count << "\n";
            out << "  }";
            bool isLast = (&info == &regions.back());
            if (!isLast)
            {
                out << ",";
            }
            out << "\n";
        }
        out << "]";
    }
}

void fun()
{
    // this hack allows us to resolve printf which is used by compiled_model.o
    // Not sure why the windows linker is not resolving it anyway.
    printf("hi");
}
