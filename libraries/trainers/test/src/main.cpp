////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (trainers_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// data
#include "Dataset.h"

// functions
#include "L2Regularizer.h"
#include "LogLoss.h"
#include "SquaredLoss.h"


// trainers
#include "MeanCalculator.h"
#include "SDCATrainer.h"
#include "SGDTrainer.h"
#include "SquaredLoss.h"

// utilities
#include "testing.h"

using namespace ell;

/// Runs all tests
///

void TestSDCATrainer()
{
    data::AutoSupervisedDataset dataset;
    dataset.AddExample({ { 1.0, 0.0, 2.0, 0.0, 3.0 }, { 1.0, 1.0 } });
    dataset.AddExample({ { 0.0, 4.0, 5.0, 6.0, 7.0 }, { 1.0, -1.0 } });
    dataset.AddExample({ { 8.0, 0.0, 9.0 }, { 1.0, 1.0 } });
    dataset.AddExample({ { 0.0, 10.0 }, { 1.0, -1.0 } });

    auto trainer = trainers::MakeSDCATrainer(functions::LogLoss(), functions::L2Regularizer(), { 1.0e-4, 1.0e-8, 20, false, "XYZ" });
    trainer->SetDataset(dataset.GetAnyDataset());

    double error = 0;
    for (auto i = 0; i < 20; i++)
    {
        trainer->Update();

        // test the resulting predictor
        functions::LogLoss lossFunction;
        error = 0;
        const predictors::LinearPredictor<double>& predictor = trainer->GetPredictor();
        for (size_t j = 0; j < dataset.NumExamples(); ++j)
        {
            const data::AutoSupervisedExample& example = dataset[j];
            const auto& dataVector = example.GetDataVector();
            auto result = predictor.Predict(dataVector);
            auto loss = lossFunction(result, example.GetMetadata().label);
            error += loss;
        }
        printf("TestSDCATrainer error is %f\n", error);
    }

    testing::ProcessTest("TestSDCATrainer", error < 0.01);

    return;
}

void TestSGDTrainer()
{
    data::AutoSupervisedDataset dataset;
    // sepal.length, sepal.width, petal.length => petal.width for IRIS
    dataset.AddExample({ { 5.1, 1.4 }, { 1.0, 2 } });
    dataset.AddExample({ { 4.9, 1.4 }, { 1.0, 2 } });
    dataset.AddExample({ { 4.7, 1.3 }, { 1.0, 2 } });
    dataset.AddExample({ { 4.6, 1.3 }, { 1.0, 2 } });
    dataset.AddExample({ { 5.0, 1.4 }, { 1.0, 2 } });
    dataset.AddExample({ { 5.4, 1.7 }, { 1.0, 4 } });
    dataset.AddExample({ { 4.6, 1.4 }, { 1.0, 3 } });
    dataset.AddExample({ { 5.0, 1.5 }, { 1.0, 2 } });
    dataset.AddExample({ { 4.4, 1.4 }, { 1.0, 2 } });
    dataset.AddExample({ { 4.9, 1.5 }, { 1.0, 1 } });
    dataset.AddExample({ { 5.4, 1.5 }, { 1.0, 2 } });
    dataset.AddExample({ { 4.8, 1.6 }, { 1.0, 2 } });
    dataset.AddExample({ { 4.8, 1.4 }, { 1.0, 1 } });
    dataset.AddExample({ { 4.3, 1.1 }, { 1.0, 1 } });
    dataset.AddExample({ { 5.8, 1.2 }, { 1.0, 2 } });
    dataset.AddExample({ { 5.7, 1.5 }, { 1.0, 4 } });
    dataset.AddExample({ { 5.4, 1.3 }, { 1.0, 4 } });
    dataset.AddExample({ { 5.1, 1.4 }, { 1.0, 3 } });

    auto trainer = trainers::MakeSGDTrainer(functions::SquaredLoss(), { 4, "XYZ" });
    trainer->SetDataset(dataset.GetAnyDataset());

    double error = 0;
    for (auto j = 0; j < 20; j++)
    {
        trainer->Update();

        // test the resulting predictor
        functions::SquaredLoss lossFunction;
        error = 0;
        const auto& predictor = trainer->GetPredictor();
        for (size_t i = 0; i < dataset.NumExamples(); ++i)
        {
            const data::AutoSupervisedExample& example = dataset[i];
            const auto& dataVector = example.GetDataVector();
            auto result = predictor.Predict(dataVector);
            auto loss = lossFunction(result, example.GetMetadata().label);
            error += loss;
        }
        printf("TestSGDTrainer error is %f\n", error);
    }

    auto weights = trainer->GetPredictor().GetWeights();
    for (size_t i = 0; i < weights.Size(); i++)
    {
        printf("weight %zu == %f\n", i, weights[i]);
    }
    auto bias = trainer->GetPredictor().GetBias();
    printf("bias == %f\n", bias);
    testing::ProcessTest("TestSDGTrainer, final cumulative error", error < 10);

    return;
}

void TestMeanCalculator()
{
    data::AutoSupervisedDataset dataset;
    dataset.AddExample({ { 1.0, 0.0, 2.0, 0.0, 3.0 }, { 1.0, 1.0 } });
    dataset.AddExample({ { 0.0, 4.0, 5.0, 6.0, 7.0 }, { 1.0, -1.0 } });
    dataset.AddExample({ { 8.0, 0.0, 9.0 }, { 1.0, 1.0 } });
    dataset.AddExample({ { 0.0, 10.0 }, { 1.0, -1.0 } });

    auto mean = trainers::CalculateMean(dataset.GetAnyDataset());

    math::RowVector<double> r{ 2.25, 3.5, 4, 1.5, 2.5 };
    testing::ProcessTest("TestMeanCalculator", mean == r);
}

int main()
{
    TestSDCATrainer();
    TestSGDTrainer();
    TestMeanCalculator();
}
