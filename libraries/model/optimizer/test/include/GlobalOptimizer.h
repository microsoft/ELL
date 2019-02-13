////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GlobalOptimizer.h (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cost.h"
#include "CostModel.h"
#include "Environment.h"
#include "GlobalOptimizerOptions.h"
#include "Objective.h"

#include <model/include/Submodel.h>
#include <model/include/Transformation.h>

#include <memory>

namespace ell
{
namespace model
{
    namespace optimizer
    {
        /// Base class for a global model optimizer
        class Optimizer
        {
        public:
            template <typename CostModelType, typename ObjectiveType>
            Optimizer(CostModelType costModel, ObjectiveType objective);
            virtual ~Optimizer() = default;

            virtual Submodel Optimize(const Submodel& submodel, const Environment& environment, const OptimizerOptions& options);

        protected:
            Objective::ObjectiveValue GetObjectiveDelta(const Transformation& transformation, const Submodel& submodel, const Environment& environment);
            Objective::ObjectiveValue EvaluateObjective(const Cost& cost) const;
            virtual void Reset(){};
            virtual bool IsDone() const = 0;

            // TODO: need to jointly choose transformation and submodel...
            virtual const Transformation& GetTransformation() = 0;
            virtual bool KeepTransformation(const Objective::ObjectiveValue& objectiveDelta) const = 0;

            std::unique_ptr<model::optimizer::CostModel> _costModel;
            std::unique_ptr<model::optimizer::Objective> _objective;
        };
    } // namespace optimizer
} // namespace model
} // namespace ell

#pragma region implementation
namespace ell
{
namespace model
{
    namespace optimizer
    {
        template <typename CostModelType, typename ObjectiveType>
        Optimizer::Optimizer(CostModelType costModel, ObjectiveType objective) :
            _costModel(std::make_unique<CostModelType>(std::move(costModel))),
            _objective(std::make_unique<ObjectiveType>(std::move(objective)))
        {
        }
    } // namespace optimizer
} // namespace model
} // namespace ell
#pragma endregion implementation
