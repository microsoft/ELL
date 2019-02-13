////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleObjectives.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ExampleObjectives.h"

using namespace ell::model;

// SimpleObjective
double SimpleObjective::Evaluate(const optimizer::Cost& cost) const
{
    return cost.GetCostComponent("runtime").GetValue();
}
