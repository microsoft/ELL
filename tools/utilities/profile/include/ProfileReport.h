////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProfileReport.h (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef COMPILED_ELL_PROFILER
#include "compiled_model.h"
#else
#include "IRModelProfiler.h"
#include "IRProfiler.h"
using ELL_ProfileRegionInfo = ell::emitters::ProfileRegionInfo;
using ELL_NodeInfo = ell::model::NodeInfo;
using ELL_PerformanceCounters = ell::model::PerformanceCounters;
#endif // COMPILED_ELL_PROFILER

// stl
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

//
// Enum for the output format
//
enum class ProfileOutputFormat
{
    text,
    json
};

std::string EncodeJSONString(const std::string& str);

void WriteUserComment(const std::string& comment, ProfileOutputFormat format, std::ostream& out);
void WriteModelStatistics(const ELL_PerformanceCounters* modelStats, ProfileOutputFormat format, std::ostream& out);
void WriteNodeStatistics(std::vector<std::pair<ELL_NodeInfo, ELL_PerformanceCounters>>& nodeInfo, std::vector<std::pair<ELL_NodeInfo, ELL_PerformanceCounters>>& nodeTypeInfo, ProfileOutputFormat format, std::ostream& out);
void WriteRegionStatistics(std::vector<ELL_ProfileRegionInfo>& regions, ProfileOutputFormat format, std::ostream& out);
