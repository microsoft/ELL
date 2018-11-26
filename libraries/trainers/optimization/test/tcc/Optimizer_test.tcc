////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Optimizer_test.tcc (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RandomExampleSet.h"

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
void TestSDCARegressionConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, double biasVariance, double inputVariance, double outputVariance)
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
    auto examples = GetRegressionExampleSet(count, inputVariance, outputVariance, solution, randomEngine);

    // create optimizer
    auto optimizer = MakeSDCAOptimizer<VectorSolution<double, true>>(examples, lossFunction, regularizer, parameters);
    optimizer.PerformEpochs(epochs);
    double dualityGap = optimizer.GetSolutionInfo().DualityGap();

    // perform test
    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);
    std::string regularizerName = typeid(RegularizerType).name();
    regularizerName = regularizerName.substr(regularizerName.find_last_of(":") + 1);

    testing::ProcessTest("TestSDCARegressionConvergence <" + lossName + ", " + regularizerName + ">", dualityGap <= parameters.desiredDualityGap);

    //std::cout << solution.GetBias() << "; " << solution.GetVector() << std::endl;
    //std::cout << optimizer.GetSolution().GetBias() << "; " << optimizer.GetSolution().GetVector() << "\t" << optimizer.GetSolutionInfo().primalObjective << std::endl;
}

#include <iostream>

// assert that the duality gap tends to zero
template <typename LossFunctionType, typename RegularizerType>
void TestSDCAClassificationConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, double biasVariance, double marginMean, double inputVariance)
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
    auto examples = GetClassificationExampleSet(count, marginMean, inputVariance, solution, randomEngine);

    // create optimizer
    auto optimizer = MakeSDCAOptimizer<VectorSolution<double, true>>(examples, lossFunction, regularizer, parameters);
    optimizer.PerformEpochs(epochs);
    double dualityGap = optimizer.GetSolutionInfo().DualityGap();

    // perform test
    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);
    std::string regularizerName = typeid(RegularizerType).name();
    regularizerName = regularizerName.substr(regularizerName.find_last_of(":") + 1);

    testing::ProcessTest("TestSDCAClassificationConvergence <" + lossName + ", " + regularizerName + ">", dualityGap <= parameters.desiredDualityGap);

    //std::cout << solution.GetBias() << "; " << solution.GetVector() << std::endl;
    //std::cout << optimizer.GetSolution().GetBias() << "; " << optimizer.GetSolution().GetVector() << "\t" << optimizer.GetSolutionInfo().primalObjective << std::endl;
}
