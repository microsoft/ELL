////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleOptimizers.h (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cost.h"
#include "GlobalOptimizer.h"

#include <model/include/Transformation.h>

#include <utilities/include/Exception.h>

//
// Example optimizers for testing
//
class TrivialOptimizer : public ell::model::optimizer::Optimizer
{
public:
    TrivialOptimizer();

protected:
    bool IsDone() const override { return true; };
    const ell::model::optimizer::Transformation& GetTransformation() override { throw ell::utilities::LogicException(ell::utilities::LogicExceptionErrors::notImplemented); };
    bool KeepTransformation(const ell::model::optimizer::Objective::ObjectiveValue& objectiveDelta) const override { return false; }
};
