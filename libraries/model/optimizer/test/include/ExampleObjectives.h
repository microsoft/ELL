////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleObjectives.h (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cost.h"
#include "Objective.h"

class SimpleObjective : public ell::model::optimizer::Objective
{
public:
    double Evaluate(const ell::model::optimizer::Cost& cost) const override;
};
