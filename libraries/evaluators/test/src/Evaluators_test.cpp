////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Evaluators_test.cpp (evaluators_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Evaluators_test.h"

// data
#include "Dataset.h"

// common
#include "MakeEvaluator.h"
#include "TrainerArguments.h"

// predictors
#include "LinearPredictor.h"

// evaluators
#include "Evaluator.h"

// testing
#include "testing.h"

// stl
#include <iostream>

namespace ell
{
void TestEvaluators()
{
    // Create a dataset
    using ExampleType = data::DenseSupervisedDataset::DatasetExampleType;
    data::DenseSupervisedDataset dataset;
    dataset.AddExample(ExampleType{ { 1.0, 1.0 }, data::WeightLabel{ 1.0, -1.0 } });
    dataset.AddExample(ExampleType{ { -1.0, -1.0 }, data::WeightLabel{ 1.0, 1.0 } });

    // get evaluator
    evaluators::EvaluatorParameters evaluatorParams{ 1, true };
    common::LossArguments lossArguments;
    lossArguments.lossFunction = common::LossArguments::LossFunction::squared;
    lossArguments.lossFunctionParameter = 0.0;
    predictors::LinearPredictor predictor({ 1.0, 1.0 }, 1.0);
    auto evaluator = common::MakeEvaluator<predictors::LinearPredictor>(dataset.GetAnyDataset(), evaluatorParams, lossArguments);

    evaluator->Evaluate(predictor);
    evaluator->Evaluate(predictor);

    evaluator->Print(std::cout);
    std::cout << "\n";
    std::cout << "Goodness: " << evaluator->GetGoodness() << std::endl;
    testing::ProcessTest("Evaluator sanity check", !testing::IsEqual(evaluator->GetGoodness(), 0.0, 1e-8));
}
}
