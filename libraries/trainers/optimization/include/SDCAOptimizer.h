////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SDCAOptimizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// math
#include "Vector.h"

// stl
#include <cstddef>
#include <memory>
#include <random>
#include <string>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        /// <summary> Parameters for the stochastic gradient descent optimizer. </summary>
        struct SDCAOptimizerParameters
        {
            double regularization;
            double desiredDualityGap = 0.0;
            bool permuteData = true;
            std::string randomSeedString = "abc123";
        };

        /// <summary> Information about the current solution found by SDCA. </summary>
        struct SDCASolutionInfo
        {
            double primalObjective = 0;
            double dualObjective = 0;
            size_t numEpochsPerformed = 0;

            double DualityGap() const { return primalObjective - dualObjective; }
        };

        /// <summary> </summary>
        ///
        /// <typeparam name="SolutionType"> Solution type. </typeparam>
        /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
        /// <typeparam name="RegularizerType"> Regularizer type. </typeparam>
        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        class SDCAOptimizer
        {
        public:
            using ExampleType = typename SolutionType::ExampleType;
            using ExampleSetType = typename SolutionType::ExampleSetType;

            /// <summary> Constructs an instance of SDCAOptimizer. </summary>
            ///
            /// <param name="examples"> The set of examples. </param>
            /// <param name="lossFunction"> The loss function. </param>
            /// <param name="regularizer"> The regularizer. </param>
            /// <param name="parameters"> Trainer parameters. </param>
            SDCAOptimizer(std::shared_ptr<const ExampleSetType> examples, LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters);

            /// <summary> Perform one or more epochs on the examples. </summary>
            ///
            /// <param name="count"> The number of epochs to perform. </param>
            void PerformEpochs(size_t count = 1);

            /// <summary> Returns the current solution. </summary>
            const SolutionType& GetSolution() const { return _w; }

            /// <summary> Returns the current solution info. </summary>
            const SDCASolutionInfo& GetSolutionInfo() const { return _solutionInfo; }

        private:
            std::shared_ptr<const ExampleSetType> _examples;
            LossFunctionType _lossFunction;
            RegularizerType _regularizer;
            std::default_random_engine _randomEngine;

            SolutionType _w;
            SolutionType _v;

            struct ExampleInfo
            {
                typename SolutionType::AuxiliaryDoubleType dual;
                double norm2Squared = 0;
            };

            std::vector<ExampleInfo> _exampleInfo;
            SDCASolutionInfo _solutionInfo;

            double _t = 0;
            double _lambda;
            double _normalizedInverseLambda;
            double _desiredDualityGap;
            bool _permuteData;

            void Step(ExampleType example, ExampleInfo& exampleInfo);
            void ComputeObjectives();
        };

        /// <summary> Convenience function for constructing an SDCA optimizer. </summary>
        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType> MakeSDCAOptimizer(std::shared_ptr<const typename SolutionType::ExampleSetType> examples, LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters);
    } // namespace optimization
} // namespace trainers
} // namespace ell

#include "../tcc/SDCAOptimizer.tcc"
