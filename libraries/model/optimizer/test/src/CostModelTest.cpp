////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CostModelTest.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CostModelTest.h"
#include "CostModel.h"
#include "Environment.h"
#include "ExampleCostModels.h"
#include "ExampleTransformations.h"
#include "OptimizerTestUtil.h"

#include <model/include/Submodel.h>

#include <testing/include/testing.h>

#include <iostream>

using namespace ell::model;
using namespace ell::model::optimizer;
using namespace ell::testing;

//
// CostModel class tests
//

void TestExampleCostModels()
{
    TestSimpleCostModel();
}

void TestSimpleCostModel()
{
    Environment environment({ "host" });
    auto m = GetSimpleSubmodel();

    SimpleCostModel costModel1;
    auto cost1 = costModel1.GetCost(m, environment);
    ProcessTest("SimpleCostModel", cost1.GetCostComponent("runtime").GetValue() == 0);

    Cost cost;
    cost["runtime"] = HeuristicCostValue(5);
    CostDatabase costDatabase;
    costDatabase.AddCostMeasurement(m, environment, cost);
    SimpleCostModel costModel2(costDatabase);

    auto cost2 = costModel2.GetCost(m, environment);
    ProcessTest("SimpleCostModel", cost2.GetCostComponent("runtime").GetValue() == 5);
}
