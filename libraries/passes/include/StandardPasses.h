////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StandardPasses.h (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/optimizer/include/ModelOptimizer.h>
#include <model/optimizer/include/ModelOptimizerOptions.h>
#include <model/optimizer/include/OptimizationPassRegistry.h>

namespace ell
{
namespace passes
{
    /// <summary> Add standard passes to the registry </summary>
    void AddStandardPassesToRegistry();
} // namespace passes
} // namespace ell
