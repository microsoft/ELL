////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Optimizer_test.h (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <trainers/optimization/include/SDCAOptimizer.h>

/// <summary> Tests that the SDCA duality gap tends to zero in a regression setting after a sufficient number of epochs.</summary>
template <typename LossFunctionType, typename RegularizerType>
void TestSDCARegressionConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, double earlyStopping, double biasVariance, double inputVariance, double outputVariance);

/// <summary> Tests that the SDCA duality gap tends to zero in a classification setting after a sufficient number of epochs.</summary>
template <typename LossFunctionType, typename RegularizerType>
void TestSDCAClassificationConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, double earlyStopping, double biasVariance, double marginMean, double inputVariance);

/// <summary> Tests that SDCA resets correctly.</summary>
template <typename LossFunctionType, typename RegularizerType>
void TestSDCAReset(LossFunctionType lossFunction, RegularizerType regularizer);

#pragma region implementation

#include "../include/RandomDataset.h"

#include <trainers/optimization/include/GetSparseSolution.h>
#include <trainers/optimization/include/IndexedContainer.h>
#include <trainers/optimization/include/OptimizationExample.h>
#include <trainers/optimization/include/SDCAOptimizer.h>
#include <trainers/optimization/include/VectorSolution.h>

#include <testing/include/testing.h>

#include <memory>
#include <string>

using namespace ell;
using namespace ell::trainers::optimization;

// assert that the duality gap tends to zero
template <typename LossFunctionType, typename RegularizerType>
void TestSDCARegressionConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, double earlyStopping, double biasVariance, double inputVariance, double outputVariance)
{
    size_t count = 500;
    size_t size = 17;
    size_t epochs = 50;

    std::string randomSeedString = "GoodLuckMan";
    std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
    std::default_random_engine randomEngine(seed);

    // create random solution
    VectorSolution<double, true> solution(size);
    std::normal_distribution<double> biasDistribution(0, biasVariance);
    solution.GetBias() = biasDistribution(randomEngine);

    std::uniform_int_distribution<int> vectorDistribution(-1, 1);
    solution.GetVector().Generate([&]() { return vectorDistribution(randomEngine); });

    // create random dataset
    auto examples = GetRegressionDataset(count, inputVariance, outputVariance, solution, randomEngine);

    // create optimizer
    auto optimizer = MakeSDCAOptimizer<VectorSolution<double, true>>(examples, lossFunction, regularizer, parameters);
    optimizer.Update(epochs, earlyStopping);
    double dualityGap = optimizer.GetSolutionInfo().DualityGap();

    // perform test
    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);
    std::string regularizerName = typeid(RegularizerType).name();
    regularizerName = regularizerName.substr(regularizerName.find_last_of(":") + 1);

    testing::ProcessTest("TestSDCARegressionConvergence <" + lossName + ", " + regularizerName + ">", dualityGap <= earlyStopping);
}

#include <iostream>

// assert that the duality gap tends to zero
template <typename LossFunctionType, typename RegularizerType>
void TestSDCAClassificationConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, double earlyStopping, double biasVariance, double marginMean, double inputVariance)
{
    size_t count = 500;
    size_t size = 17;
    size_t epochs = 50;

    std::string randomSeedString = "GoodLuckMan";
    std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
    std::default_random_engine randomEngine(seed);

    // create random solution
    VectorSolution<double, true> solution(size);
    std::normal_distribution<double> biasDistribution(0, biasVariance);
    solution.GetBias() = biasDistribution(randomEngine);

    std::uniform_int_distribution<int> vectorDistribution(-1, 1);
    solution.GetVector().Generate([&]() { return vectorDistribution(randomEngine); });

    // create random dataset
    auto examples = GetClassificationDataset(count, marginMean, inputVariance, solution, randomEngine);

    // create optimizer
    auto optimizer = MakeSDCAOptimizer<VectorSolution<double, true>>(examples, lossFunction, regularizer, parameters);
    optimizer.Update(epochs, earlyStopping);
    double dualityGap = optimizer.GetSolutionInfo().DualityGap();

    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);
    std::string regularizerName = typeid(RegularizerType).name();
    regularizerName = regularizerName.substr(regularizerName.find_last_of(":") + 1);

    testing::ProcessTest("TestSDCAClassificationConvergence <" + lossName + ", " + regularizerName + ">", dualityGap <= earlyStopping);
}

template <typename LossFunctionType, typename RegularizerType>
void TestSDCAReset(LossFunctionType lossFunction, RegularizerType regularizer)
{
    size_t count = 50;
    size_t size = 5;
    size_t epochs = 3;

    std::string randomSeedString = "GoodLuckMan";
    std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
    std::default_random_engine randomEngine(seed);

    // create random dataset
    auto examples = GetRandomDataset<double, VectorScalarExampleType<double>, VectorRefScalarExampleType<double>>(count, size, randomEngine, 0);

    // create optimizer
    auto optimizer1 = MakeSDCAOptimizer<VectorSolution<double, true>>(examples, lossFunction, regularizer, { 0.1, false });
    optimizer1.Update(epochs);
    auto solution1 = optimizer1.GetSolution();
    const auto& vector1 = solution1.GetVector();

    optimizer1.Reset();
    optimizer1.Update(epochs);
    auto solution2 = optimizer1.GetSolution();
    const auto& vector2 = solution2.GetVector();

    auto optimizer2 = SDCAOptimizer<VectorSolution<double, true>, LossFunctionType, RegularizerType>(examples);
    optimizer2.SetLossFunction(lossFunction);
    optimizer2.SetRegularizer(regularizer);
    optimizer2.SetParameters({ 0.1, false });
    optimizer2.Update(epochs);
    auto solution3 = optimizer2.GetSolution();
    const auto& vector3 = solution3.GetVector();

    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);
    std::string regularizerName = typeid(RegularizerType).name();
    regularizerName = regularizerName.substr(regularizerName.find_last_of(":") + 1);

    testing::ProcessTest("TestSDCAReset <" + lossName + ", " + regularizerName + ">", vector1 == vector2 && vector1 == vector3);
}

template <typename LossFunctionType>
void TestGetSparseSolution(LossFunctionType lossFunction, double regularizationParameter)
{
    size_t count = 500;
    size_t size = 17;

    double biasVariance = 1.0;
    double marginMean = 1.0;
    double inputVariance = 1.0;

    std::string randomSeedString = "GoodLuckMan";
    std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
    std::default_random_engine randomEngine(seed);

    // create random solution
    VectorSolution<double, true> targetSolution(size);
    std::normal_distribution<double> biasDistribution(0, biasVariance);
    targetSolution.GetBias() = biasDistribution(randomEngine);

    std::uniform_int_distribution<int> vectorDistribution(-1, 1);
    targetSolution.GetVector().Generate([&]() { return vectorDistribution(randomEngine); });

    auto examples = GetClassificationDataset(count, marginMean, inputVariance, targetSolution, randomEngine);

    auto trainedSolution1 = GetSparseSolution<VectorSolution<double, true>>(examples, lossFunction, { { 0.7, 0.75 }, { regularizationParameter } });
    double nonZeroFraction1 = trainedSolution1.GetVector().Norm0() / trainedSolution1.GetVector().Size();

    auto trainedSolution2 = GetSparseSolution<VectorSolution<double, true>>(examples, lossFunction, { { 0.45, 0.5 }, { regularizationParameter } });
    double nonZeroFraction2 = trainedSolution2.GetVector().Norm0() / trainedSolution2.GetVector().Size();

    auto trainedSolution3 = GetSparseSolution<VectorSolution<double, true>>(examples, lossFunction, { { 0.2, 0.25 }, { regularizationParameter } });
    double nonZeroFraction3 = trainedSolution3.GetVector().Norm0() / trainedSolution3.GetVector().Size();

    testing::ProcessTest("TestGetSparseSolution",
                         std::abs(nonZeroFraction1 - 0.75) < 0.1 &&
                             std::abs(nonZeroFraction2 - 0.5) < 0.1 &&
                             std::abs(nonZeroFraction3 - 0.25) < 0.1);
}

#pragma endregion implementation
