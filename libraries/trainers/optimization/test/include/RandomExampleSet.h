////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RandomExampleSet.h (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <trainers/optimization/include/IndexedContainer.h>
#include <trainers/optimization/include/OptimizationExample.h>
#include <trainers/optimization/include/VectorSolution.h>

#include <math/include/Vector.h>

#include <memory>
#include <random>

using namespace ell;
using namespace ell::trainers::optimization;

template <typename ElementType, typename VectorExampleType, typename IndexedContainerExampleType>
std::shared_ptr<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>> GetRandomExampleSet(size_t count, size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures = 0);

using VectorExampleType = Example<math::RowVector<double>, double>;
using ContainerExampleType = Example<math::ConstRowVectorReference<double>, double>;

std::shared_ptr<VectorIndexedContainer<VectorExampleType, ContainerExampleType>> GetClassificationExampleSet(size_t count, double marginMean, double marginVariance, const VectorSolution<double, true>& solution, std::default_random_engine& randomEngine);

std::shared_ptr<VectorIndexedContainer<VectorExampleType, ContainerExampleType>> GetRegressionExampleSet(size_t count, double inputVariance, double outputVariance, const VectorSolution<double, true>& solution, std::default_random_engine& randomEngine);

#pragma region implementation

using namespace ell;

template <typename ElementType, typename ExampleType>
ExampleType GetRandomExample(size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures = 0)
{
    // allocate vector
    math::RowVector<ElementType> vector(randomVectorSize + numConstantFeatures);
    vector.Fill(1);
    auto vectorView = vector.GetSubVector(0, randomVectorSize);

    // generate random values
    std::normal_distribution<double> normal(0, 200);
    vectorView.Generate([&]() { return static_cast<ElementType>(normal(randomEngine)); });

    ElementType output = randomEngine() % 2 == 0 ? static_cast<ElementType>(-1) : static_cast<ElementType>(1);

    using OutputType = typename ExampleType::OutputType;
    return ExampleType{ std::move(vector), OutputType{ output } };
}

template <typename ElementType, typename VectorExampleType, typename IndexedContainerExampleType>
std::shared_ptr<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>> GetRandomExampleSet(size_t count, size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures)
{
    auto exampleSet = std::make_shared<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>>();
    exampleSet->reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        exampleSet->push_back(GetRandomExample<ElementType, VectorExampleType>(randomVectorSize, randomEngine, numConstantFeatures));
    }
    return exampleSet;
}

#pragma endregion implementation
