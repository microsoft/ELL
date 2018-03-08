////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StandardPasses.h (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model/optimizer
#include "ModelOptimizer.h"
#include "ModelOptimizerOptions.h"
#include "OptimizationPassRegistry.h"

namespace ell
{
namespace passes
{
    /// <summary> Add standard passes to the registry </summary>
    void AddStandardPassesToRegistry();

    //
    // Individual passes
    //

    /// <summary> Add a FuseOperationsPass to the given optimizer. </summary>
    void AddFuseOperationsPass(model::ModelOptimizer& optimizer);
}
}
