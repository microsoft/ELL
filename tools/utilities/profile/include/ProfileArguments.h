////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProfileArguments.h (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace ell
{
    enum class ProfileOutputFormat
    {
        text,
        json
    };

/// <summary> Arguments for profile tool. </summary>
struct ProfileArguments
{
    std::string inputTestFile;

    std::string outputFilename = "";
    std::string timingOutputFilename = "";
    ProfileOutputFormat outputFormat = ProfileOutputFormat::text;
    std::string outputComment = "";

    int numIterations = 1;
    int numBurnInIterations = 0;
    bool filterTrivialNodes = true;
    bool summaryOnly = false;

    bool optimize = true;
    bool useBlas = true;
    bool fuseLinearOperations = true;
};

/// <summary> Arguments for parsed print. </summary>
struct ParsedProfileArguments : public ProfileArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser);
};
}
