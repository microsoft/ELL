////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Global optimizer tests
//

#include "CostModelTest.h"
#include "CostTest.h"
#include "EnvironmentTest.h"
#include "ObjectiveTest.h"
#include "OptimizerOptionsTest.h"
#include "OptimizerTest.h"
#include "TransformationTest.h"

#include <testing/include/testing.h>

using namespace ell::testing;

int main()
{
    TestExampleCostModels();

    TestCosts();

    TestEnvironment();

    TestObjectives();

    TestOptimizerOptions();

    TestOptimizers();

    TestTransformations();

    return GetExitCode();
}
