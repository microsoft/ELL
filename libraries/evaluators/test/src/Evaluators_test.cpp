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

// predictors
#include "LinearPredictor.h"

// evaluators
#include "AUCAggregator.h"
#include "Evaluator.h"
#include "LossAggregator.h"

// functions
#include "SquaredLoss.h"

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
    predictors::LinearPredictor<double> predictor({ 1.0, 1.0 }, 1.0);
    auto evaluator = evaluators::MakeEvaluator<predictors::LinearPredictor<double>>(dataset.GetAnyDataset(), evaluatorParams, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(functions::SquaredLoss()));

    evaluator->Evaluate(predictor);
    evaluator->Evaluate(predictor);

    evaluator->Print(std::cout);
    std::cout << "\n";
    std::cout << "Goodness: " << evaluator->GetGoodness() << std::endl;
    testing::ProcessTest("Evaluator sanity check", !testing::IsEqual(evaluator->GetGoodness(), 0.0, 1e-8));
}
}
