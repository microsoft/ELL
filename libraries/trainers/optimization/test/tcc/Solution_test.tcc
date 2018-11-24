////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Solution_test.tcc (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RandomExampleSet.h"

// optimization
#include "IndexedContainer.h"
#include "MatrixSolution.h"
#include "MultivariateLoss.h"
#include "OptimizationExample.h"
#include "SDCAOptimizer.h"
#include "SGDOptimizer.h"
#include "VectorSolution.h"

// testing
#include "testing.h"

// math
#include "Vector.h"

// stl
#include <memory>
#include <string>

using namespace ell;
using namespace ell::trainers::optimization;

template <typename T>
using VectorScalarExampleType = Example<math::RowVector<T>, T>;

template <typename T>
using VectorRefScalarExampleType = Example<math::ConstRowVectorReference<T>, T>;

template <typename T>
using VectorVectorExampleType = Example<math::RowVector<T>, math::RowVector<T>>;

template <typename T>
using VectorRefVectorRefExampleType = Example<math::ConstRowVectorReference<T>, math::ConstRowVectorReference<T>>;

// Run the SGD trainer with four different solution types and confirm that the result is identical
template <typename RealType, typename LossFunctionType, typename RegularizerType>
void TestSolutionEquivalenceSGD(double regularizationParameter)
{
    std::string randomSeedString = "54321blastoff";
    std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
    std::default_random_engine randomEngine;

    const size_t numExamples = 5;
    const size_t exampleSize = 12;

    randomEngine.seed(seed);
    auto examples1 = GetRandomExampleSet<RealType, VectorScalarExampleType<RealType>, VectorRefScalarExampleType<RealType>>(numExamples, exampleSize, randomEngine, 1);

    randomEngine.seed(seed);
    auto examples2 = GetRandomExampleSet<RealType, VectorScalarExampleType<RealType>, VectorRefScalarExampleType<RealType>>(numExamples, exampleSize, randomEngine, 0);

    randomEngine.seed(seed);
    auto examples3 = GetRandomExampleSet<RealType, VectorVectorExampleType<RealType>, VectorRefVectorRefExampleType<RealType>>(numExamples, exampleSize, randomEngine, 1);

    randomEngine.seed(seed);
    auto examples4 = GetRandomExampleSet<RealType, VectorVectorExampleType<RealType>, VectorRefVectorRefExampleType<RealType>>(numExamples, exampleSize, randomEngine, 0);

    // setup four equivalent optimizers
    auto optimizer1 = MakeSGDOptimizer<VectorSolution<RealType>>(examples1, LossFunctionType{}, { regularizationParameter });
    optimizer1.PerformEpochs();
    const auto& solution1 = optimizer1.GetSolution();
    const auto& vector1 = solution1.GetVector();

    auto optimizer2 = MakeSGDOptimizer<VectorSolution<RealType, true>>(examples2, LossFunctionType{}, { regularizationParameter });
    optimizer2.PerformEpochs();
    const auto& solution2 = optimizer2.GetSolution();
    const auto& vector2 = solution2.GetVector();

    auto optimizer3 = MakeSGDOptimizer<MatrixSolution<RealType>>(examples3, MultivariateLoss<LossFunctionType>{}, { regularizationParameter });
    optimizer3.PerformEpochs();
    const auto& solution3 = optimizer3.GetSolution();
    const auto& vector3 = solution3.GetMatrix().GetColumn(0);

    auto optimizer4 = MakeSGDOptimizer<MatrixSolution<RealType, true>>(examples4, MultivariateLoss<LossFunctionType>{}, { regularizationParameter });
    optimizer4.PerformEpochs();
    const auto& solution4 = optimizer4.GetSolution();
    const auto& vector4 = solution4.GetMatrix().GetColumn(0);

    double comparisonTolerance = 1.0e-7;

    std::string realName = typeid(RealType).name();
    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);

    // test if the two solutions are identical
    testing::ProcessTest("TestSolutionEquivalenceSGD (v1 == v2) <" + realName + ", " + lossName + ">", vector1.GetSubVector(0, exampleSize).IsEqual(vector2, comparisonTolerance));
    testing::ProcessTest("TestSolutionEquivalenceSGD (v1.last == b2) <" + realName + ", " + lossName + ">", testing::IsEqual(vector1[exampleSize], solution2.GetBias(), comparisonTolerance));
    testing::ProcessTest("TestSolutionEquivalenceSGD (v1 == v3) <" + realName + ", " + lossName + ">", vector1.IsEqual(vector3, comparisonTolerance));
    testing::ProcessTest("TestSolutionEquivalenceSGD (v2 == v4) <" + realName + ", " + lossName + ">", vector2.IsEqual(vector4, comparisonTolerance));
    testing::ProcessTest("TestSolutionEquivalenceSGD (b2 == b4) <" + realName + ", " + lossName + ">", testing::IsEqual(solution4.GetBias()[0], solution2.GetBias(), comparisonTolerance));
}

// Run the SDCA trainer with four different solution types and confirm that the result is identical
template <typename RealType, typename LossFunctionType, typename RegularizerType>
void TestSolutionEquivalenceSDCA(double regularizationParameter)
{
    std::string randomSeedString = "54321blastoff";
    std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
    std::default_random_engine randomEngine;

    const size_t numExamples = 5;
    const size_t exampleSize = 7;

    randomEngine.seed(seed);
    auto examples1 = GetRandomExampleSet<RealType, VectorScalarExampleType<RealType>, VectorRefScalarExampleType<RealType>>(numExamples, exampleSize, randomEngine, 1);

    randomEngine.seed(seed);
    auto examples2 = GetRandomExampleSet<RealType, VectorScalarExampleType<RealType>, VectorRefScalarExampleType<RealType>>(numExamples, exampleSize, randomEngine, 0);

    randomEngine.seed(seed);
    auto examples3 = GetRandomExampleSet<RealType, VectorVectorExampleType<RealType>, VectorRefVectorRefExampleType<RealType>>(numExamples, exampleSize, randomEngine, 1);

    randomEngine.seed(seed);
    auto examples4 = GetRandomExampleSet<RealType, VectorVectorExampleType<RealType>, VectorRefVectorRefExampleType<RealType>>(numExamples, exampleSize, randomEngine, 0);

    // setup four equivalent optimizers
    auto optimizer1 = MakeSDCAOptimizer<VectorSolution<RealType>>(examples1, LossFunctionType{}, RegularizerType{}, { regularizationParameter });
    optimizer1.PerformEpochs();
    const auto& solution1 = optimizer1.GetSolution();
    const auto& vector1 = solution1.GetVector();

    auto optimizer2 = MakeSDCAOptimizer<VectorSolution<RealType, true>>(examples2, LossFunctionType{}, RegularizerType{}, { regularizationParameter });
    optimizer2.PerformEpochs();
    const auto& solution2 = optimizer2.GetSolution();
    const auto& vector2 = solution2.GetVector();

    auto optimizer3 = MakeSDCAOptimizer<MatrixSolution<RealType>>(examples3, MultivariateLoss<LossFunctionType>{}, RegularizerType{}, { regularizationParameter });
    optimizer3.PerformEpochs();
    const auto& solution3 = optimizer3.GetSolution();
    const auto& vector3 = solution3.GetMatrix().GetColumn(0);

    auto optimizer4 = MakeSDCAOptimizer<MatrixSolution<RealType, true>>(examples4, MultivariateLoss<LossFunctionType>{}, RegularizerType{}, { regularizationParameter });
    optimizer4.PerformEpochs();
    const auto& solution4 = optimizer4.GetSolution();
    const auto& vector4 = solution4.GetMatrix().GetColumn(0);

    double comparisonTolerance = 1.0e-6;

    std::string realName = typeid(RealType).name();
    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);

    // test if the two solutions are identical
    testing::ProcessTest("TestSolutionEquivalenceSDCA (v1 == v2) <" + realName + ", " + lossName + ">", vector1.GetSubVector(0, exampleSize).IsEqual(vector2, comparisonTolerance));
    testing::ProcessTest("TestSolutionEquivalenceSDCA (v1.last == b2) <" + realName + ", " + lossName + ">", testing::IsEqual(vector1[exampleSize], solution2.GetBias(), comparisonTolerance));
    testing::ProcessTest("TestSolutionEquivalenceSDCA (v1 == v3) <" + realName + ", " + lossName + ">", vector1.IsEqual(vector3, comparisonTolerance));
    testing::ProcessTest("TestSolutionEquivalenceSDCA (v2 == v4) <" + realName + ", " + lossName + ">", vector2.IsEqual(vector4, comparisonTolerance));
    testing::ProcessTest("TestSolutionEquivalenceSDCA (b2 == b4) <" + realName + ", " + lossName + ">", testing::IsEqual(solution4.GetBias()[0], solution2.GetBias(), comparisonTolerance));
};
