////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleCostModels.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ExampleCostModels.h"

#include <emitters/include/CompilerOptions.h>

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>

#include <utilities/include/Exception.h>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace ell::model;

// SimpleCostModel
SimpleCostModel::SimpleCostModel()
{
}

SimpleCostModel::SimpleCostModel(CostDatabase perfData) :
    _perfData(perfData)
{
}

optimizer::Cost SimpleCostModel::NullCost() const
{
    return {};
}

bool SimpleCostModel::HasCost(const Submodel& submodel, const optimizer::Environment& environment) const
{
    return _perfData.HasCostMeasurement(submodel, environment);
}

optimizer::Cost SimpleCostModel::GetCost(const Submodel& submodel, const optimizer::Environment& environment) const
{
    if (_perfData.HasCostMeasurement(submodel, environment))
    {
        return _perfData.GetCostMeasurement(submodel, environment);
    }

    return NullCost();
}
