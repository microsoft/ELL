////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Regularizer_test.cpp (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Regularizer_test.h"
#include "RandomExampleSet.h"

// optimization
#include "ElasticNetRegularizer.h"
#include "HuberLoss.h"
#include "MatrixSolution.h"
#include "MaxRegularizer.h"
#include "L2Regularizer.h"
#include "OptimizationExample.h"
#include "SDCAOptimizer.h"
#include "VectorSolution.h"

// testing
#include "testing.h"

// stl
#include <random>

using namespace ell::trainers::optimization;

using VectorScalarExampleType = Example<math::RowVector<double>, double>;
using VectorRefScalarExampleType = Example<math::ConstRowVectorReference<double>, double>;

void TestRegularizerEquivalence(double regularizationParameter)
{
    std::string randomSeedString = "54321blastoff";
    std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
    std::default_random_engine randomEngine;
    randomEngine.seed(seed);

    const size_t numExamples = 20;
    const size_t exampleSize = 10;

    auto examples = GetRandomExampleSet<double, VectorScalarExampleType, VectorRefScalarExampleType>(numExamples, exampleSize, randomEngine);

    // setup three equivalent optimizers
    auto optimizer1 = MakeSDCAOptimizer<VectorSolution<double>>(examples, HuberLoss{}, L2Regularizer{}, { regularizationParameter });
    optimizer1.PerformEpochs();
    const auto& solution1 = optimizer1.GetSolution();
    const auto& vector1 = solution1.GetVector();

    auto optimizer2 = MakeSDCAOptimizer<VectorSolution<double>>(examples, HuberLoss{}, ElasticNetRegularizer{ 0 }, { regularizationParameter });
    optimizer2.PerformEpochs();
    const auto& solution2 = optimizer2.GetSolution();
    const auto& vector2 = solution2.GetVector();

    auto optimizer3 = MakeSDCAOptimizer<VectorSolution<double>>(examples, HuberLoss{}, MaxRegularizer{ 0 }, { regularizationParameter });
    optimizer3.PerformEpochs();
    const auto& solution3 = optimizer3.GetSolution();
    const auto& vector3 = solution3.GetVector();

    const double tolerance = 1.0e-8;

    testing::ProcessTest("TestRegularizerEquivalence", vector1.IsEqual(vector2, tolerance) && vector1.IsEqual(vector3, tolerance));
}