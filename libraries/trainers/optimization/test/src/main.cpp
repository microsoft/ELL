////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "L2Regularizer.h"
#include "LogLoss.h"
#include "MatrixExampleSet.h"
#include "MatrixSolution.h"
#include "MultivariateLossAdapter.h"
#include "SDCAOptimizer.h"
#include "SGDOptimizer.h"
#include "SquareLoss.h"
#include "VectorSolution.h"
#include "testing.h"

// stl
#include <iostream>
#include <memory>

using namespace ell;
using namespace ell::trainers::optimization;

template <typename SolutionType, typename LossFunctionType, typename ExampleSetType>
double CalculateEmpiricalLoss(const SolutionType& solution, const LossFunctionType& lossFunction, const ExampleSetType& examples)
{
    double totalLoss = 0;
    for (size_t i = 0; i < examples.Size(); ++i)
    {
        auto example = examples.Get(i);
        auto prediction = example.input * solution;
        const auto& output = example.output;
        double exampleLoss = lossFunction.Value(prediction, output);
        totalLoss += exampleLoss;
    }
    return totalLoss / examples.Size();
}

template <typename ElementType, typename ExampleType>
ExampleType GetRandomExample(size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures = 0)
{
    // allocate vector
    math::RowVector<ElementType> vector(randomVectorSize + numConstantFeatures);
    vector.Fill(1);
    auto vectorView = vector.GetSubVector(0, randomVectorSize);

    // generate random values
    std::normal_distribution<double> normal(0,200);
    vectorView.Generate([&]() { return static_cast<ElementType>(normal(randomEngine)); });

    ElementType label = randomEngine() % 2 == 0 ? static_cast<ElementType>(-1) : static_cast<ElementType>(1);

    using OutputType = typename ExampleType::OutputType;
    return ExampleType{ std::move(vector), OutputType{ label } };
}

template <typename ElementType, typename VectorExampleType, typename IndexedContainerExampleType>
std::shared_ptr<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>> GetRandomExampleSet(size_t count, size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures = 0)
{
    auto exampleSet = std::make_shared<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>>();
    exampleSet->reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        exampleSet->push_back(GetRandomExample<ElementType, VectorExampleType>(randomVectorSize, randomEngine, numConstantFeatures));
    }
    return exampleSet;
}

// assert that the first SGD epochs cause the empirical loss to decrease monotonically
template <typename SolutionType, typename InputType, typename OutputType, typename LossFunctionType>
void TestSGDMonotonicity(LossFunctionType lossFunction, SGDOptimizerParameters parameters)
{
    using ExampleType = Example<InputType, OutputType>;
    using SolutionExampleType = typename SolutionType::ExampleType;
    using ExampleSetType = VectorIndexedContainer<ExampleType, SolutionExampleType>;

    ExampleSetType exampleSet =
        { ExampleType{ InputType{ 3, -3, 1, 1 }, OutputType{ 1 } },
          ExampleType{ InputType{ 1, -3, -1, 1 }, OutputType{ 1 } },
          ExampleType{ InputType{ 3, -3, 1, 1 }, OutputType{ 1 } },
          ExampleType{ InputType{ 1, -1, 1, 1 }, OutputType{ -1 } },
          ExampleType{ InputType{ 1, -3, 1, 1 }, OutputType{ -1 } },
          ExampleType{ InputType{ 1, -1, 1, 1 }, OutputType{ -1 } } };

    auto examples = std::make_shared<const ExampleSetType>(std::move(exampleSet));

    // create optimizer
    auto optimizer = MakeSGDOptimizer<SolutionType>(examples, lossFunction, parameters);

    // run one epoch and calculate the empirical loss
    optimizer.PerformEpochs();
    double loss = CalculateEmpiricalLoss(optimizer.GetSolution(), lossFunction, *examples);

    // check if the empirical loss decreases monotonically
    bool monotonic = true;
    for (int i = 0; i < 8; ++i)
    {
        optimizer.PerformEpochs();
        const auto& solution = optimizer.GetSolution();
        double newLoss = CalculateEmpiricalLoss(solution, lossFunction, *examples);
        if (newLoss >= loss)
        {
            monotonic = false;
        }
        loss = newLoss;
    }

    testing::ProcessTest("TestSGDMonotonicity", monotonic);
};

// assert that the duality gap goes to zero
template <typename SolutionType, typename InputType, typename OutputType, typename LossFunctionType, typename RegularizerType>
void TestSDCAConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters)
{
    using ExampleType = Example<InputType, OutputType>;
    using SolutionExampleType = typename SolutionType::ExampleType;
    using ExampleSetType = VectorIndexedContainer<ExampleType, SolutionExampleType>;

    ExampleSetType exampleSet =
        { ExampleType{ InputType{ 1, 0, 2, 0, 3 }, OutputType{ 1 } },
          ExampleType{ InputType{ 0, 4, 5, 6, 7 }, OutputType{ -1 } },
          ExampleType{ InputType{ 8, 0, 9, 0, 0 }, OutputType{ 1 } },
          ExampleType{ InputType{ 0, 10, 0, 0, 0 }, OutputType{ -1 } } };

    auto examples = std::make_shared<const ExampleSetType>(std::move(exampleSet));

    // create optimizer
    auto optimizer = MakeSDCAOptimizer<SolutionType>(examples, lossFunction, regularizer, parameters);
    optimizer.PerformEpochs(50);
    double dualityGap = optimizer.GetSolutionInfo().DualityGap();
    testing::ProcessTest("TestSDCAConvergence", dualityGap <= parameters.desiredDualityGap);
};

template <typename T>
using VectorScalarExampleType = Example<math::RowVector<T>, T>;

template <typename T>
using VectorRefScalarExampleType = Example<math::ConstRowVectorReference<T>, T>;

template <typename T>
using VectorVectorExampleType = Example<math::RowVector<T>, math::RowVector<T>>;

template <typename T>
using VectorRefVectorRefExampleType = Example<math::ConstRowVectorReference<T>, math::ConstRowVectorReference<T>>;

// Run the SGD trainer with four different solution types and confirm that the result is identical
template <typename RealType, typename LossFunctionType>
void TestSGDSideBySide(double regularizationParameter)
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

    // setup three equivalent optimizers
    auto optimizer1 = MakeSGDOptimizer<VectorSolution<RealType>>(examples1, LossFunctionType{}, { regularizationParameter });
    optimizer1.PerformEpochs();
    const auto& solution1 = optimizer1.GetSolution();
    const auto& vector1 = solution1.GetVector();

    auto optimizer2 = MakeSGDOptimizer<VectorSolution<RealType, true>>(examples2, LossFunctionType{}, { regularizationParameter });
    optimizer2.PerformEpochs();
    const auto& solution2 = optimizer2.GetSolution();
    const auto& vector2 = solution2.GetVector();

    auto optimizer3 = MakeSGDOptimizer<MatrixSolution<RealType>>(examples3, MultivariateLossAdapter<LossFunctionType>{}, { regularizationParameter });
    optimizer3.PerformEpochs();
    const auto& solution3 = optimizer3.GetSolution();
    const auto& vector3 = solution3.GetMatrix().GetColumn(0);

    auto optimizer4 = MakeSGDOptimizer<MatrixSolution<RealType, true>>(examples4, MultivariateLossAdapter<LossFunctionType>{}, { regularizationParameter });
    optimizer4.PerformEpochs();
    const auto& solution4 = optimizer4.GetSolution();
    const auto& vector4 = solution4.GetMatrix().GetColumn(0);

    double comparisonTolerance = 1.0e-7;

    // test if the two solutions are identical
    testing::ProcessTest("TestSGDSideBySide (v1 == v2)", vector1.GetSubVector(0, exampleSize).IsEqual(vector2, comparisonTolerance));
    testing::ProcessTest("TestSGDSideBySide (v1.last == b2)", testing::IsEqual(vector1[exampleSize], solution2.GetBias(), comparisonTolerance));
    testing::ProcessTest("TestSGDSideBySide (v1 == v3)", vector1.IsEqual(vector3, comparisonTolerance));
    testing::ProcessTest("TestSGDSideBySide (v2 == v4)", vector2.IsEqual(vector4, comparisonTolerance));
    testing::ProcessTest("TestSGDSideBySide (b2 == b4)", testing::IsEqual(solution4.GetBias()[0], solution2.GetBias(), comparisonTolerance));
}

// Run the SDCA trainer with four different solution types and confirm that the result is identical
template <typename RealType, typename LossFunctionType>
void TestSDCASideBySide(double regularizationParameter)
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

    // setup three equivalent optimizers
    auto optimizer1 = MakeSDCAOptimizer<VectorSolution<RealType>>(examples1, LossFunctionType{}, L2Regularizer{}, { regularizationParameter });
    optimizer1.PerformEpochs();
    const auto& solution1 = optimizer1.GetSolution();
    const auto& vector1 = solution1.GetVector();

    auto optimizer2 = MakeSDCAOptimizer<VectorSolution<RealType, true>>(examples2, LossFunctionType{}, L2Regularizer{}, { regularizationParameter });
    optimizer2.PerformEpochs();
    const auto& solution2 = optimizer2.GetSolution();
    const auto& vector2 = solution2.GetVector();

    auto optimizer3 = MakeSDCAOptimizer<MatrixSolution<RealType>>(examples3, MultivariateLossAdapter<LossFunctionType>{}, L2Regularizer{}, { regularizationParameter });
    optimizer3.PerformEpochs();
    const auto& solution3 = optimizer3.GetSolution();
    const auto& vector3 = solution3.GetMatrix().GetColumn(0);

    auto optimizer4 = MakeSDCAOptimizer<MatrixSolution<RealType, true>>(examples4, MultivariateLossAdapter<LossFunctionType>{}, L2Regularizer{}, { regularizationParameter });
    optimizer4.PerformEpochs();
    const auto& solution4 = optimizer4.GetSolution();
    const auto& vector4 = solution4.GetMatrix().GetColumn(0);

    double comparisonTolerance = 1.0e-6;

    // test if the two solutions are identical
    testing::ProcessTest("TestSDCASideBySide (v1 == v2)", vector1.GetSubVector(0, exampleSize).IsEqual(vector2, comparisonTolerance));
    testing::ProcessTest("TestSDCASideBySide (v1.last == b2)", testing::IsEqual(vector1[exampleSize], solution2.GetBias(), comparisonTolerance));
    testing::ProcessTest("TestSDCASideBySide (v1 == v3)", vector1.IsEqual(vector3, comparisonTolerance));
    testing::ProcessTest("TestSDCASideBySide (v2 == v4)", vector2.IsEqual(vector4, comparisonTolerance));
    testing::ProcessTest("TestSDCASideBySide (b2 == b4)", testing::IsEqual(solution4.GetBias()[0], solution2.GetBias(), comparisonTolerance));
};

void TestMatrixExampleSet()
{
    size_t numRows = 10;
    size_t numInputColumns = 7;
    size_t numOutputColumns = 4;

    // generate two random matrices
    std::default_random_engine randomEngine;
    std::normal_distribution<double> normal;

    math::RowMatrix<double> input(numRows, numInputColumns);
    input.Generate([&]() { return normal(randomEngine); });

    math::RowMatrix<double> output(numRows, numOutputColumns);
    output.Generate([&]() { return normal(randomEngine); });

    // use the two matrices to create an example set
    auto examples = std::make_shared<MatrixExampleSet<double>>( std::move(input), std::move(output) );
    
    // train an SGD optimizer on the example set
    auto optimizer = MakeSGDOptimizer<MatrixSolution<double>>(examples, MultivariateLossAdapter<LogLoss>{}, { 0.0001 });
    optimizer.PerformEpochs();
}

int main()
{
    // SGDOptimizer

    TestSGDMonotonicity<VectorSolution<double>, math::RowVector<double>, double>(LogLoss{}, { 0.0001 });
    TestSGDMonotonicity<VectorSolution<double, true>, math::RowVector<double>, double>(LogLoss{}, { 0.0001 });
    TestSGDMonotonicity<MatrixSolution<double>, math::RowVector<double>, math::RowVector<double>>(MultivariateLossAdapter<LogLoss>{}, { 0.0001 });
    TestSGDMonotonicity<MatrixSolution<double, true>, math::RowVector<double>, math::RowVector<double>>(MultivariateLossAdapter<LogLoss>{}, { 0.0001 });

    TestSGDMonotonicity<VectorSolution<float>, math::RowVector<float>, float>(LogLoss{}, { 0.0001 });
    TestSGDMonotonicity<VectorSolution<float, true>, math::RowVector<float>, float>(LogLoss{}, { 0.0001 });
    TestSGDMonotonicity<MatrixSolution<float>, math::RowVector<float>, math::RowVector<float>>(MultivariateLossAdapter<LogLoss>{}, { 0.0001 });
    TestSGDMonotonicity<MatrixSolution<float, true>, math::RowVector<float>, math::RowVector<float>>(MultivariateLossAdapter<LogLoss>{}, { 0.0001 });

    TestSGDMonotonicity<VectorSolution<int>, math::RowVector<int>, int>(LogLoss{}, { 0.0001 });
    TestSGDMonotonicity<VectorSolution<int, true>, math::RowVector<int>, int>(LogLoss{}, { 0.0001 });
    TestSGDMonotonicity<MatrixSolution<int>, math::RowVector<int>, math::RowVector<int>>(MultivariateLossAdapter<LogLoss>{}, { 0.0001 });
    TestSGDMonotonicity<MatrixSolution<int, true>, math::RowVector<int>, math::RowVector<int>>(MultivariateLossAdapter<LogLoss>{}, { 0.0001 });

    TestSGDMonotonicity<VectorSolution<double>, math::RowVector<double>, double>(SquareLoss{}, { 1.125 });
    TestSGDMonotonicity<VectorSolution<double, true>, math::RowVector<double>, double>(SquareLoss{}, { 1.125 });
    TestSGDMonotonicity<MatrixSolution<double>, math::RowVector<double>, math::RowVector<double>>(MultivariateLossAdapter<SquareLoss>{}, { 1.125 });
    TestSGDMonotonicity<MatrixSolution<double, true>, math::RowVector<double>, math::RowVector<double>>(MultivariateLossAdapter<SquareLoss>{}, { 1.125 });

    TestSGDMonotonicity<VectorSolution<float>, math::RowVector<float>, float>(SquareLoss{}, { 1.125 });
    TestSGDMonotonicity<VectorSolution<float, true>, math::RowVector<float>, float>(SquareLoss{}, { 1.125 });
    TestSGDMonotonicity<MatrixSolution<float>, math::RowVector<float>, math::RowVector<float>>(MultivariateLossAdapter<SquareLoss>{}, { 1.125 });
    TestSGDMonotonicity<MatrixSolution<float, true>, math::RowVector<float>, math::RowVector<float>>(MultivariateLossAdapter<SquareLoss>{}, { 1.125 });

    TestSGDMonotonicity<VectorSolution<int>, math::RowVector<int>, int>(SquareLoss{}, { 1.125 });
    TestSGDMonotonicity<VectorSolution<int, true>, math::RowVector<int>, int>(SquareLoss{}, { 1.125 });
    TestSGDMonotonicity<MatrixSolution<int>, math::RowVector<int>, math::RowVector<int>>(MultivariateLossAdapter<SquareLoss>{}, { 1.125 });
    TestSGDMonotonicity<MatrixSolution<int, true>, math::RowVector<int>, math::RowVector<int>>(MultivariateLossAdapter<SquareLoss>{}, { 1.125 });

    //// SDCAOptimizer

    TestSDCAConvergence<VectorSolution<double>, math::RowVector<double>, double>(LogLoss{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<VectorSolution<double, true>, math::RowVector<double>, double>(LogLoss{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<double>, math::RowVector<double>, math::RowVector<double>>(MultivariateLossAdapter<LogLoss>{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<double, true>, math::RowVector<double>, math::RowVector<double>>(MultivariateLossAdapter<LogLoss>{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });

    TestSDCAConvergence<VectorSolution<float>, math::RowVector<float>, float>(LogLoss{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<VectorSolution<float, true>, math::RowVector<float>, float>(LogLoss{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<float>, math::RowVector<float>, math::RowVector<float>>(MultivariateLossAdapter<LogLoss>{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<float, true>, math::RowVector<float>, math::RowVector<float>>(MultivariateLossAdapter<LogLoss>{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });

    TestSDCAConvergence<VectorSolution<int>, math::RowVector<int>, int>(LogLoss{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<VectorSolution<int, true>, math::RowVector<int>, int>(LogLoss{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<int>, math::RowVector<int>, math::RowVector<int>>(MultivariateLossAdapter<LogLoss>{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<int, true>, math::RowVector<int>, math::RowVector<int>>(MultivariateLossAdapter<LogLoss>{}, L2Regularizer{}, { 1.0e-4, 1.0e-8, false });

    TestSDCAConvergence<VectorSolution<double>, math::RowVector<double>, double>(SquareLoss{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<VectorSolution<double, true>, math::RowVector<double>, double>(SquareLoss{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<double>, math::RowVector<double>, math::RowVector<double>>(MultivariateLossAdapter<SquareLoss>{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<double, true>, math::RowVector<double>, math::RowVector<double>>(MultivariateLossAdapter<SquareLoss>{}, L2Regularizer{}, { 1.0, 1.0e-8, false });

    TestSDCAConvergence<VectorSolution<float>, math::RowVector<float>, float>(SquareLoss{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<VectorSolution<float, true>, math::RowVector<float>, float>(SquareLoss{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<float>, math::RowVector<float>, math::RowVector<float>>(MultivariateLossAdapter<SquareLoss>{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<float, true>, math::RowVector<float>, math::RowVector<float>>(MultivariateLossAdapter<SquareLoss>{}, L2Regularizer{}, { 1.0, 1.0e-8, false });

    TestSDCAConvergence<VectorSolution<int>, math::RowVector<int>, int>(SquareLoss{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<VectorSolution<int, true>, math::RowVector<int>, int>(SquareLoss{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<int>, math::RowVector<int>, math::RowVector<int>>(MultivariateLossAdapter<SquareLoss>{}, L2Regularizer{}, { 1.0, 1.0e-8, false });
    TestSDCAConvergence<MatrixSolution<int, true>, math::RowVector<int>, math::RowVector<int>>(MultivariateLossAdapter<SquareLoss>{}, L2Regularizer{}, { 1.0, 1.0e-8, false });

    // side-by-side tests
    TestSGDSideBySide<double, LogLoss>(0.0001);
    TestSGDSideBySide<float, LogLoss>(0.0001);
    TestSGDSideBySide<int, LogLoss>(0.0001);

    TestSGDSideBySide<double, SquareLoss>(1.0e+3);
    TestSGDSideBySide<float, SquareLoss>(1.0e+3);
    TestSGDSideBySide<int, SquareLoss>(1.0e+3);

    TestSDCASideBySide<double, LogLoss>(0.0001);
    TestSDCASideBySide<float, LogLoss>(0.0001);
    TestSDCASideBySide<int, LogLoss>(0.0001);

    TestSDCASideBySide<double, SquareLoss>(10);
    TestSDCASideBySide<float, SquareLoss>(10);
    TestSDCASideBySide<int, SquareLoss>(10);

    // Matrix Example Set
    TestMatrixExampleSet();

    return testing::DidTestFail();
}
