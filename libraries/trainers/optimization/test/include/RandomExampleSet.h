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

#include "../tcc/RandomExampleSet.tcc"
