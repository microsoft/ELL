////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompilerOptions.h (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelOptimizerOptions.h"

// emitters
#include "CompilerOptions.h"

// stl
#include <string>

namespace ell
{
namespace model
{
    class Map;
    class Model;
    class Node;
    
    struct MapCompilerOptions
    {
        // map-specific compiler settings
        std::string moduleName = "ELL";
        std::string mapFunctionName = "predict";
        bool inlineNodes = false;
        bool profile = false;
        std::string sourceFunctionName;
        std::string sinkFunctionName;
        
        // optimizations
        ModelOptimizerOptions optimizerSettings;

        // lower-level emitters settings
        emitters::CompilerOptions compilerSettings;
    };
}
}

