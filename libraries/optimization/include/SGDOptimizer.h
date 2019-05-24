////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDOptimizer.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>

namespace ell
{
namespace optimization
{
    /// <summary> Parameters for the stochastic gradient descent optimizer. </summary>
    struct SGDOptimizerParameters
    {
        double regularizationParameter;
        std::string randomSeedString = "abc123";
    };

    /// <summary> Stochastic gradient descent optimizer. </summary>
    ///
    /// <typeparam name="SolutionType"> Solution type. </typeparam>
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    template <typename SolutionType, typename LossFunctionType>
    class SGDOptimizer
    {
    public:
        using ExampleType = typename SolutionType::ExampleType;
        using DatasetType = typename SolutionType::DatasetType;

        /// <summary> Constructor </summary>
        ///
        /// <param name="examples"> The set of examples. </param>
        /// <param name="parameters"> Optimizer parameters. </param>
        /// <param name="lossFunction"> The loss function. </param>
        SGDOptimizer(std::shared_ptr<const DatasetType> examples, LossFunctionType lossFunction, SGDOptimizerParameters parameters);

        /// <summary> Perform one or more epochs on the examples. </summary>
        ///
        /// <param name="epochs"> The number of epochs to perform. </param>
        void Update(size_t epochs = 1);

        /// <summary> Returns the current solution to the optimization problem. </summary>
        const SolutionType& GetSolution() const { return _averagedW; }

    private:
        void Step(ExampleType example);

        std::shared_ptr<const DatasetType> _examples;
        LossFunctionType _lossFunction;
        std::default_random_engine _randomEngine;
        SolutionType _lastW;
        SolutionType _averagedW;
        double _t = 0;
        double _lambda;
    };

    /// <summary> Convenience function for constructing an SGD optimizer. </summary>
    template <typename SolutionType, typename LossFunctionType>
    SGDOptimizer<SolutionType, LossFunctionType> MakeSGDOptimizer(std::shared_ptr<const typename SolutionType::DatasetType> examples, LossFunctionType lossFunction, SGDOptimizerParameters parameters);

} // namespace optimization
} // namespace ell

#pragma region implementation

namespace ell
{
namespace optimization
{
    /// <summary> </summary>
    template <typename SolutionType, typename LossFunctionType>
    SGDOptimizer<SolutionType, LossFunctionType>::SGDOptimizer(std::shared_ptr<const DatasetType> examples, LossFunctionType lossFunction, SGDOptimizerParameters parameters) :
        _examples(examples),
        _lossFunction(std::move(lossFunction)),
        _lambda(parameters.regularizationParameter)
    {
        if (examples.get() == nullptr || examples->Size() == 0)
        {
            throw OptimizationException("Empty dataset");
        }

        // check that all the outputs are compatible with the loss
        for (size_t i = 0; i < examples->Size(); ++i)
        {
            auto example = examples->Get(i);

            if (!_lossFunction.VerifyOutput(example.output))
            {
                throw OptimizationException("Discovered an output that is incompatible with the chosen loss function");
            }
        }

        // setup random engine
        std::seed_seq seed(parameters.randomSeedString.begin(), parameters.randomSeedString.end());
        _randomEngine.seed(seed);

        auto example = examples->Get(0);
        _lastW.Resize(example.input, example.output);
        _averagedW.Resize(example.input, example.output);
    }

    template <typename SolutionType, typename LossFunctionType>
    void SGDOptimizer<SolutionType, LossFunctionType>::Update(size_t epochs)
    {
        std::vector<size_t> permutation(_examples->Size());
        std::iota(permutation.begin(), permutation.end(), 0);

        // epochs
        for (size_t e = 0; e < epochs; ++e)
        {
            // generate random permutation
            std::shuffle(permutation.begin(), permutation.end(), _randomEngine);

            // process each example
            for (size_t index : permutation)
            {
                Step(_examples->Get(index));
            }
        }
    }

    template <typename SolutionType, typename LossFunctionType>
    void SGDOptimizer<SolutionType, LossFunctionType>::Step(ExampleType example)
    {
        const auto& x = example.input;
        const auto& y = example.output;
        double weight = example.weight;

        ++_t;

        // predict
        auto p = x * _lastW;

        // calculate the loss derivative
        auto derivative = _lossFunction.Derivative(p, y);
        derivative *= -weight / (_lambda * _t);

        // update the solution
        double inverseT = 1.0 / _t;
        _lastW = _lastW * (1.0 - inverseT) + Transpose(x) * derivative;
        _averagedW = _averagedW * (1.0 - inverseT) + _lastW * inverseT;
    }

    template <typename SolutionType, typename LossFunctionType>
    SGDOptimizer<SolutionType, LossFunctionType> MakeSGDOptimizer(std::shared_ptr<const typename SolutionType::DatasetType> examples, LossFunctionType lossFunction, SGDOptimizerParameters parameters)
    {
        return SGDOptimizer<SolutionType, LossFunctionType>(examples, lossFunction, parameters);
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
