////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationPassRegistry.cpp (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizationPassRegistry.h"

// model
#include "ModelOptimizer.h"
#include "ModelOptimizerOptions.h"

// stl
#include <functional>
#include <string>
#include <vector>

namespace ell
{
namespace model
{
    namespace
    {
        std::vector<OptimizationPassInfo> _passes;
    }

    void OptimizationPassRegistry::AddPassesToOptimizer(ModelOptimizer& optimizer, const ModelOptimizerOptions& settings)
    {
        for(const auto& pass: _passes)
        {
            if(pass.isValidFunction(settings))
            {
                optimizer.AddPass(pass.createFunction());
            }
        }
    }

    void OptimizationPassRegistry::AddPass(const OptimizationPassInfo& passInfo)
    {
        _passes.push_back(passInfo);
    }
}
}
