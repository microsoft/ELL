////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace common
{
    /// <summary> A struct that holds general command line parameters for training algorithms. </summary>
    struct TrainerArguments
    {
        /// <summary> A random seed. </summary>
        std::string randomSeedString = ""; 

        /// <summary> If `true`, more output is generated. </summary>
        bool verbose = false; 
    };

    /// <summary> A version of TrainerArguments that adds its members to the command line parser. </summary>
    struct ParsedTrainerArguments : public TrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };
}
