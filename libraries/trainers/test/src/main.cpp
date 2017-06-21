////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (trainers_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// common
#include "MakeTrainer.h"

// data
#include "Dataset.h"

// trainers
#include "SDCATrainer.h"
#include "MeanCalculator.h"

// utilities
#include "testing.h"

using namespace ell;

/// Runs all tests
///

void TestSDCATrainer()
{
    data::AutoSupervisedDataset dataset;
    dataset.AddExample({ { 1.0, 0.0, 2.0, 0.0, 3.0 },{ 1.0, 1.0 } });
    dataset.AddExample({ { 0.0, 4.0, 5.0, 6.0, 7.0 },{ 1.0, -1.0 } });
    dataset.AddExample({ { 8.0, 0.0, 9.0 },{ 1.0, 1.0 } });
    dataset.AddExample({ { 0.0, 10.0 },{ 1.0, -1.0 } });

    auto trainer = common::MakeSDCATrainer({ common::LossFunctionArguments::LossFunction::log }, { 1.0e-4, 1.0e-8, 20, false, "XYZ" });
    trainer->SetDataset(dataset.GetAnyDataset());
    trainer->Update();
    trainer->Update();
    trainer->Update();
    trainer->Update();
    trainer->Update();
    trainer->Update();
    trainer->Update();
    trainer->Update();

    return;
}

void TestMeanCalculator()
{
    data::AutoSupervisedDataset dataset;
    dataset.AddExample({ { 1.0, 0.0, 2.0, 0.0, 3.0 },{ 1.0, 1.0 } });
    dataset.AddExample({ { 0.0, 4.0, 5.0, 6.0, 7.0 },{ 1.0, -1.0 } });
    dataset.AddExample({ { 8.0, 0.0, 9.0 },{ 1.0, 1.0 } });
    dataset.AddExample({ { 0.0, 10.0 },{ 1.0, -1.0 } });

    auto mean = trainers::CalculateMean(dataset.GetAnyDataset());

    math::RowVector<double> r{ 2.25, 3.5, 4, 1.5, 2.5 };
    testing::ProcessTest("TestMeanCalculator", mean == r);
}

int main()
{
    TestSDCATrainer();
    TestMeanCalculator();
}
