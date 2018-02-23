////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationPassRegistry.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OptimizationPass.h"

// stl
#include <functional>
#include <memory>
#include <string>

namespace ell
{
namespace model
{
    class ModelOptimizer;
    struct ModelOptimizerOptions;

    struct OptimizationPassInfo
    {
        std::string name;
        std::function<bool(const ModelOptimizerOptions& settings)> isValidFunction;
        std::function<std::unique_ptr<OptimizationPass>()> createFunction;
    };

    /// <summary> Global registry for optimization passes </summary>
    class OptimizationPassRegistry
    {
    public:
        static void AddPassesToOptimizer(ModelOptimizer& optimizer, const ModelOptimizerOptions& settings);

        static void AddPass(const OptimizationPassInfo& passInfo);
    };
}
}
