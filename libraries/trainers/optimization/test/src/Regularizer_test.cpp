////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Regularizer_test.cpp (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Regularizer_test.h"
#include "RandomDataset.h"

#include <trainers/optimization/include/ElasticNetRegularizer.h>
#include <trainers/optimization/include/HuberLoss.h>
#include <trainers/optimization/include/L2Regularizer.h>
#include <trainers/optimization/include/MatrixSolution.h>
#include <trainers/optimization/include/MaxRegularizer.h>
#include <trainers/optimization/include/OptimizationExample.h>
#include <trainers/optimization/include/SDCAOptimizer.h>
#include <trainers/optimization/include/VectorSolution.h>

#include <testing/include/testing.h>

#include <random>

using namespace ell::trainers::optimization;

void TestRegularizerEquivalence(double regularizationParameter)
{
    std::string randomSeedString = "54321blastoff";
    std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
    std::default_random_engine randomEngine;
    randomEngine.seed(seed);

    const size_t numExamples = 20;
    const size_t exampleSize = 10;

    auto examples = GetRandomDataset<double, VectorScalarExampleType<double>, VectorRefScalarExampleType<double>>(numExamples, exampleSize, randomEngine);

    // setup three equivalent optimizers
    auto optimizer1 = MakeSDCAOptimizer<VectorSolution<double>>(examples, HuberLoss{}, L2Regularizer{}, { regularizationParameter });
    optimizer1.Update();
    const auto& solution1 = optimizer1.GetSolution();
    const auto& vector1 = solution1.GetVector();

    auto optimizer2 = MakeSDCAOptimizer<VectorSolution<double>>(examples, HuberLoss{}, ElasticNetRegularizer{ 0 }, { regularizationParameter });
    optimizer2.Update();
    const auto& solution2 = optimizer2.GetSolution();
    const auto& vector2 = solution2.GetVector();

    auto optimizer3 = MakeSDCAOptimizer<VectorSolution<double>>(examples, HuberLoss{}, MaxRegularizer{ 0 }, { regularizationParameter });
    optimizer3.Update();
    const auto& solution3 = optimizer3.GetSolution();
    const auto& vector3 = solution3.GetVector();

    const double tolerance = 1.0e-8;

    testing::ProcessTest("TestRegularizerEquivalence", vector1.IsEqual(vector2, tolerance) && vector1.IsEqual(vector3, tolerance));
}