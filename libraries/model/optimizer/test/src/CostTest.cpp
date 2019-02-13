////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CostTest.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CostTest.h"
#include "Cost.h"

#include <testing/include/testing.h>

using namespace ell::model::optimizer;
using namespace ell::testing;

//
// Cost class tests
//
void TestCosts()
{
    Cost perfCost;
    perfCost["runtime"] = HeuristicCostValue(5);
    ProcessTest("Testing PerfCost value", perfCost.GetCostComponent("runtime").GetValue() == 5);
    ProcessTest("Testing PerfCost value", perfCost.GetCostComponent("somethingelse").GetValue() == 0);
}
