////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Evaluators_test.cpp (evaluators_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Evaluators_test.h"

#include <data/include/Dataset.h>

#include <predictors/include/LinearPredictor.h>

#include <evaluators/include/AUCAggregator.h>
#include <evaluators/include/Evaluator.h>
#include <evaluators/include/LossAggregator.h>

#include <functions/include/SquaredLoss.h>

#include <testing/include/testing.h>

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
} // namespace ell
