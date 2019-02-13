////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GlobalOptimizer.cpp (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GlobalOptimizer.h"

#include <utilities/include/Exception.h>

namespace ell
{
using namespace emitters;

namespace model
{
    namespace optimizer
    {
        Objective::ObjectiveValue Optimizer::GetObjectiveDelta(const Transformation& transformation, const Submodel& submodel, const Environment& environment)
        {
            if (_costModel->HasCost(submodel, environment))
            {
                auto preTransformCost = _costModel->GetCost(submodel, environment);
                auto preTransformObjective = EvaluateObjective(preTransformCost);
                ModelTransformer transformer;
                TransformContext context;
                auto newSubmodel = transformation.Transform(submodel, transformer, context);
                if (_costModel->HasCost(newSubmodel, environment))
                {
                    auto postTransformCost = _costModel->GetCost(newSubmodel, environment);
                    auto postTransformObjective = EvaluateObjective(postTransformCost);
                    return postTransformObjective - preTransformObjective;
                }
            }
            return {};
        }

        Objective::ObjectiveValue Optimizer::EvaluateObjective(const Cost& cost) const
        {
            return _objective->Evaluate(cost);
        }

        Submodel Optimizer::Optimize(const Submodel& submodel, const Environment& environment, const OptimizerOptions& options)
        {
            Submodel currentSubmodel(submodel);
            Reset();
            while (!IsDone())
            {
                ModelTransformer transformer;
                TransformContext context;
                const auto& t = GetTransformation();
                auto cost = GetObjectiveDelta(t, currentSubmodel, environment);
                if (KeepTransformation(cost))
                {
                    currentSubmodel = t.Transform(currentSubmodel, transformer, context);
                }
            }
            return currentSubmodel;
        }
    } // namespace optimizer
} // namespace model
} // namespace ell
