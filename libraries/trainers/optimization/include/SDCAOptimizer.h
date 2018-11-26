////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SDCAOptimizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"
#include "Expression.h"

#include <math/include/Vector.h>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>

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

#pragma region implementation

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::SDCAOptimizer(std::shared_ptr<const ExampleSetType> examples, LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters) :
            _examples(examples),
            _lossFunction(lossFunction),
            _regularizer(regularizer)
        {
            if (!examples || examples->Size() == 0)
            {
                throw OptimizationException("Empty dataset");
            }

            // set parameters
            _lambda = parameters.regularization;
            _desiredDualityGap = parameters.desiredDualityGap;
            _permuteData = parameters.permuteData;

            size_t numExamples = examples->Size();
            _normalizedInverseLambda = 1.0 / (numExamples * parameters.regularization);

            // set up random engine
            std::seed_seq seed(parameters.randomSeedString.begin(), parameters.randomSeedString.end());
            _randomEngine.seed(seed);

            // resize data structures according to examples
            auto firstExample = examples->Get(0);
            _w.Resize(firstExample.input, firstExample.output);
            _v.Resize(firstExample.input, firstExample.output);
            _exampleInfo.resize(numExamples);

            // initialize the per-example info, check that outputs are compatible with the loss, and compute primal objective
            double primalSum = 0;
            for (size_t i = 0; i < numExamples; ++i)
            {
                auto example = examples->Get(i);

                if (!_lossFunction.VerifyOutput(example.output))
                {
                    throw OptimizationException("Discovered an output that is incompatible with the chosen loss function");
                }

                // cache the norm of the example
                double norm2Squared = _w.GetNorm2SquaredOf(example.input);
                _exampleInfo[i].norm2Squared = norm2Squared;

                // initialize the dual
                _w.InitializeAuxiliaryVariable(_exampleInfo[i].dual);

                // compute the primal objective
                auto prediction = example.input * _w;
                primalSum += _lossFunction.Value(prediction, example.output);
            }

            _solutionInfo.primalObjective = primalSum / numExamples + _lambda * _regularizer.Value(_w);
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::PerformEpochs(size_t count)
        {
            std::vector<size_t> permutation(_examples->Size());
            std::iota(permutation.begin(), permutation.end(), 0);

            // epochs
            for (size_t e = 0; e < count; ++e)
            {
                // early exit
                if (_solutionInfo.DualityGap() <= _desiredDualityGap)
                {
                    break;
                }

                // generate random permutation
                if (_permuteData)
                {
                    std::shuffle(permutation.begin(), permutation.end(), _randomEngine);
                }

                // process each example
                for (size_t index : permutation)
                {
                    Step(_examples->Get(index), _exampleInfo[index]);
                }

                _solutionInfo.numEpochsPerformed++;
                ComputeObjectives();
            }
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::Step(ExampleType example, ExampleInfo& exampleInfo)
        {
            const double tolerance = 1.0e-8;

            auto& dual = exampleInfo.dual;

            auto lipschitz = exampleInfo.norm2Squared * _normalizedInverseLambda;
            if (lipschitz < tolerance)
            {
                return;
            }

            auto prediction = example.input * _w;
            prediction /= lipschitz;
            prediction += dual;

            auto newDual = _lossFunction.ConjugateProx(1.0 / lipschitz, prediction, example.output);
            dual -= newDual;
            dual *= _normalizedInverseLambda;

            _v += Transpose(example.input) * dual;
            _regularizer.ConjugateGradient(_v, _w);
            exampleInfo.dual = newDual;
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::ComputeObjectives()
        {
            double primalSum = 0;
            double dualSum = 0;

            for (size_t i = 0; i < _examples->Size(); ++i)
            {
                auto example = _examples->Get(i);

                auto prediction = example.input * _w;
                primalSum += _lossFunction.Value(prediction, example.output);

                dualSum += _lossFunction.Conjugate(_exampleInfo[i].dual, example.output);
            }

            _solutionInfo.primalObjective = primalSum / _examples->Size() + _lambda * _regularizer.Value(_w);
            _solutionInfo.dualObjective = -dualSum / _examples->Size() - _lambda * _regularizer.Conjugate(_v);
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType> MakeSDCAOptimizer(std::shared_ptr<const typename SolutionType::ExampleSetType> examples, LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters)
        {
            return SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>(examples, lossFunction, regularizer, parameters);
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
