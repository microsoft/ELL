////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerOptions.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TargetDevice.h"

namespace ell
{
namespace emitters
{
    /// <summary> List of possible BLAS implementations (for enabling implementation-specific features). </summary>
    enum class BlasType
    {
        unknown = 0,
        openBLAS,
        atlas
    };
    
    /// <summary> Standard compiler switches. </summary>
    struct CompilerOptions
    {
        bool unrollLoops = false;
        bool inlineOperators = true;
        bool allowVectorInstructions = false;
        int vectorWidth = 4;
        bool useBlas = true;
        BlasType blasType = BlasType::unknown;
        bool optimize = true;
        bool includeDiagnosticInfo = false;
        bool parallelize = false;
        bool useThreadPool = true;
        int maxThreads = 4;
        bool debug = false;

        TargetDevice targetDevice;
    };
}
}
