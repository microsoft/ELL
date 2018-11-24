////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RandomExampleSet.cpp (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RandomExampleSet.h"

// stl
#include <cassert>

using namespace ell;

std::shared_ptr<VectorIndexedContainer<VectorExampleType, ContainerExampleType>> GetClassificationExampleSet(size_t count, double marginMean, double marginVariance, const VectorSolution<double, true>& solution, std::default_random_engine& randomEngine)
{
    size_t size = solution.GetVector().Size();

    // use solution to create a mean for the positive example cloud
    math::RowVector<double> positiveMean(size);
    positiveMean.CopyFrom(solution.GetVector().Transpose());
    positiveMean *= (marginMean - solution.GetBias()) / solution.GetVector().Norm2Squared();
    assert(std::abs(solution.Multiply(positiveMean) - marginMean) <= 1.0e-7);

    // use solution create a mean for the negative example cloud
    math::RowVector<double> negativeMean(size);
    negativeMean.CopyFrom(solution.GetVector().Transpose());
    negativeMean *= -(marginMean + solution.GetBias()) / solution.GetVector().Norm2Squared();
    assert(std::abs(solution.Multiply(negativeMean) + marginMean) <= 1.0e-7);

    // distribution for the example clouds
    std::normal_distribution<double> inputDistribution(0, marginVariance / size);

    // distribution for binary labels
    std::bernoulli_distribution labelDistribution(0.5);

    // create examples
    auto exampleSet = std::make_shared<VectorIndexedContainer<VectorExampleType, ContainerExampleType>>();
    exampleSet->reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        math::RowVector<double> input(size);
        input.Generate([&]() { return inputDistribution(randomEngine); });

        double output;
        if (labelDistribution(randomEngine))
        {
            input += positiveMean;
            output = 1.0;
        }
        else
        {
            input += negativeMean;
            output = -1.0;
        }

        exampleSet->push_back(VectorExampleType{ std::move(input), output });
    }
    return exampleSet;
}

std::shared_ptr<VectorIndexedContainer<VectorExampleType, ContainerExampleType>> GetRegressionExampleSet(size_t count, double inputVariance, double outputVariance, const VectorSolution<double, true>& solution, std::default_random_engine& randomEngine)
{
    size_t size = solution.GetVector().Size();

    std::normal_distribution<double> inputDistribution(0, inputVariance);
    std::normal_distribution<double> outputDistribution(0, outputVariance);

    // randomly draw the sample mean
    math::RowVector<double> mean(size);
    mean.Generate([&]() { return inputDistribution(randomEngine); });

    auto exampleSet = std::make_shared<VectorIndexedContainer<VectorExampleType, ContainerExampleType>>();
    exampleSet->reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        math::RowVector<double> input(size);
        input.Generate([&]() { return inputDistribution(randomEngine); });
        input += mean;

        double output = solution.Multiply(input) + outputDistribution(randomEngine);

        exampleSet->push_back(VectorExampleType{ std::move(input), output });
    }
    return exampleSet;
}
