////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleOptimizers.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ExampleOptimizers.h"
#include "ExampleCostModels.h"
#include "ExampleObjectives.h"

TrivialOptimizer::TrivialOptimizer() :
    ell::model::optimizer::Optimizer(SimpleCostModel{}, SimpleObjective{}){};
