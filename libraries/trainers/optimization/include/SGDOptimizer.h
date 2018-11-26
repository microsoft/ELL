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
#include <vector>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        /// <summary> Parameters for the stochastic gradient descent optimizer. </summary>
        struct SGDOptimizerParameters
        {
            double regularization;
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
            using ExampleSetType = typename SolutionType::ExampleSetType;

            /// <summary> Constructor </summary>
            ///
            /// <param name="examples"> The set of examples. </param>
            /// <param name="parameters"> Optimizer parameters. </param>
            /// <param name="lossFunction"> The loss function. </param>
            SGDOptimizer(std::shared_ptr<const ExampleSetType> examples, LossFunctionType lossFunction, SGDOptimizerParameters parameters);

            /// <summary> Perform one or more epochs on the examples. </summary>
            ///
            /// <param name="count"> The number of epochs to perform. </param>
            void PerformEpochs(size_t count = 1);

            /// <summary> Returns the current solution to the optimization problem. </summary>
            const SolutionType& GetSolution() const { return _averagedW; }

        private:
            void Step(ExampleType example);

            std::shared_ptr<const ExampleSetType> _examples;
            LossFunctionType _lossFunction;
            std::default_random_engine _randomEngine;
            SolutionType _lastW;
            SolutionType _averagedW;
            double _t = 0;
            double _lambda;
        };

        /// <summary> Convenience function for constructing an SGD optimizer. </summary>
        template <typename SolutionType, typename LossFunctionType>
        SGDOptimizer<SolutionType, LossFunctionType> MakeSGDOptimizer(std::shared_ptr<const typename SolutionType::ExampleSetType> examples, LossFunctionType lossFunction, SGDOptimizerParameters parameters);

    } // namespace optimization
} // namespace trainers
} // namespace ell

#include "../tcc/SGDOptimizer.tcc"
