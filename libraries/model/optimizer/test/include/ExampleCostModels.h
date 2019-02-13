////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleCostModels.h (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cost.h"
#include "CostDatabase.h"
#include "CostModel.h"

#include <model/include/Submodel.h>
#include <model/include/Transformation.h>

#include <functional>
#include <unordered_map>

//
// Example cost models for testing
//
class SimpleCostModel : public ell::model::optimizer::CostModel
{
public:
    SimpleCostModel();
    SimpleCostModel(const SimpleCostModel& other) = default;
    SimpleCostModel(SimpleCostModel&& other) = default;
    SimpleCostModel(CostDatabase perfData);

    bool HasCost(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment) const override;
    ell::model::optimizer::Cost GetCost(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment) const override;

private:
    ell::model::optimizer::Cost NullCost() const;

    CostDatabase _perfData;
};
