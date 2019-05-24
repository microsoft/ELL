////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RandomDataset.h (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <optimization/include/IndexedContainer.h>
#include <optimization/include/OptimizationExample.h>
#include <optimization/include/VectorSolution.h>

#include <math/include/Vector.h>

#include <memory>
#include <random>

using namespace ell;
using namespace ell::optimization;

template <typename ElementType, typename VectorExampleType, typename IndexedContainerExampleType>
std::shared_ptr<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>> GetRandomDataset(size_t count, size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures = 0);

template <typename ElementType, typename VectorExampleType, typename IndexedContainerExampleType>
std::shared_ptr<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>> GetRandomDataset(size_t count, size_t inputSize, size_t outputSize, std::default_random_engine& randomEngine, size_t numConstantFeatures = 0);

using VectorExampleType = Example<math::RowVector<double>, double>;
using ContainerExampleType = Example<math::ConstRowVectorReference<double>, double>;

std::shared_ptr<VectorIndexedContainer<VectorExampleType, ContainerExampleType>> GetClassificationDataset(size_t count, double marginMean, double marginVariance, const VectorSolution<double, true>& solution, std::default_random_engine& randomEngine);

std::shared_ptr<VectorIndexedContainer<VectorExampleType, ContainerExampleType>> GetRegressionDataset(size_t count, double inputVariance, double outputVariance, const VectorSolution<double, true>& solution, std::default_random_engine& randomEngine);

template <typename T>
using VectorScalarExampleType = Example<math::RowVector<T>, T>;

template <typename T>
using VectorRefScalarExampleType = Example<math::ConstRowVectorReference<T>, T>;

template <typename T>
using VectorVectorExampleType = Example<math::RowVector<T>, math::RowVector<T>>;

template <typename T>
using VectorRefVectorRefExampleType = Example<math::ConstRowVectorReference<T>, math::ConstRowVectorReference<T>>;

#pragma region implementation

using namespace ell;

template <typename ElementType, typename VectorType>
VectorType GetRandomVector(size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures = 0)
{
    // allocate vector
    math::RowVector<ElementType> vector(randomVectorSize + numConstantFeatures);
    vector.Fill(1);
    auto vectorView = vector.GetSubVector(0, randomVectorSize);

    // generate random values
    std::normal_distribution<double> normal(0, 200);
    vectorView.Generate([&]() { return static_cast<ElementType>(normal(randomEngine)); });
    return vector;
}

template <typename ElementType, typename ExampleType>
ExampleType GetRandomExample(size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures = 0)
{
    auto input = GetRandomVector<ElementType, math::RowVector<ElementType>>(randomVectorSize, randomEngine, numConstantFeatures);
    ElementType output = randomEngine() % 2 == 0 ? static_cast<ElementType>(-1) : static_cast<ElementType>(1);
    using OutputType = typename ExampleType::OutputType;
    return ExampleType{ input, OutputType{ output } };
}

template <typename ElementType, typename ExampleType>
ExampleType GetRandomExample(size_t inputSize, size_t outputSize, std::default_random_engine& randomEngine, size_t numConstantFeatures)
{
    auto input = GetRandomVector<ElementType, math::RowVector<ElementType>>(inputSize, randomEngine, numConstantFeatures);
    auto output = GetRandomVector<ElementType, math::RowVector<ElementType>>(outputSize, randomEngine);
    return { input, output };
}

template <typename ElementType, typename VectorExampleType, typename IndexedContainerExampleType>
std::shared_ptr<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>> GetRandomDataset(size_t count, size_t randomVectorSize, std::default_random_engine& randomEngine, size_t numConstantFeatures)
{
    auto exampleSet = std::make_shared<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>>();
    exampleSet->reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        exampleSet->push_back(GetRandomExample<ElementType, VectorExampleType>(randomVectorSize, randomEngine, numConstantFeatures));
    }
    return exampleSet;
}

template <typename ElementType, typename VectorExampleType, typename IndexedContainerExampleType>
std::shared_ptr<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>> GetRandomDataset(size_t count, size_t inputSize, size_t outputSize, std::default_random_engine& randomEngine, size_t numConstantFeatures)
{
    auto exampleSet = std::make_shared<VectorIndexedContainer<VectorExampleType, IndexedContainerExampleType>>();
    exampleSet->reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        exampleSet->push_back(GetRandomExample<ElementType, VectorExampleType>(inputSize, outputSize, randomEngine, numConstantFeatures));
    }
    return exampleSet;
}

#pragma endregion implementation
