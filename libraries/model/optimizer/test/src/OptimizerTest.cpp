////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizerTest.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizerTest.h"
#include "Environment.h"
#include "ExampleCostModels.h"
#include "ExampleObjectives.h"
#include "ExampleOptimizers.h"
#include "ExampleTransformations.h"
#include "OptimizerTestUtil.h"
#include "SequentialOptimizer.h"

#include <testing/include/testing.h>

#include <utilities/include/Exception.h>

#include <iostream>

using namespace ell;
using namespace ell::emitters;
using namespace ell::model;
using namespace ell::model::optimizer;
using namespace ell::testing;

void TestOptimizers()
{
    TestTrivialOptimizer();
    NoFailOnUnimplemented(TestSequentialOptimizer);
}

void TestTrivialOptimizer()
{
    Submodel m = GetSimpleSubmodel();
    TargetDevice t = GetTargetDevice("pi3");
    Environment env(t);
    OptimizerOptions opt;

    TrivialOptimizer optimizer;
    auto newSubmodel = optimizer.Optimize(m, env, opt);

    ProcessTest("TrivialOptimizerResultSameSize", SubmodelsAreSameSize(m, newSubmodel));
}

void TestSequentialOptimizer()
{
    Submodel m = GetCombineNodesTestSubmodel();
    TargetDevice t = GetTargetDevice("pi3");
    Environment env(t);
    OptimizerOptions opt;

    SimpleCostModel costModel;
    SequentialOptimizer optimizer(std::move(costModel), SimpleObjective{});

    optimizer.AddTransformation(TrivialTransformation());
    optimizer.AddTransformation(CombineNodesTransformation());
    optimizer.AddTransformation(AddMetadataToOutputTransformation());

    auto newSubmodel = optimizer.Optimize(m, env, opt);

    ProcessTest("SequentialOptimizerResultNotSameSize", !SubmodelsAreSameSize(m, newSubmodel));
}
