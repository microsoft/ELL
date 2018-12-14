////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SDCAOptimizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <math/include/Vector.h>

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
            double regularizationParameter;
            bool permuteData = true;
        };

        /// <summary> Information about the current solution found by SDCA. </summary>
        struct SDCASolutionInfo
        {
            double primalObjective = 0;
            double dualObjective = 0;
            size_t numEpochsPerformed = 0;

            /// <summary> Returns the duality gap, which is the difference between the primal and the dual objectives. </summary>
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
            using DatasetType = typename SolutionType::DatasetType;

            /// <summary> Constructs an instance of SDCAOptimizer and initializes it with loss, regularizer, and parameters. </summary>
            ///
            /// <param name="examples"> The set of examples. </param>
            /// <param name="lossFunction"> The loss function. </param>
            /// <param name="regularizer"> The regularizer. </param>
            /// <param name="parameters"> Trainer parameters. </param>
            /// <param name="randomSeedString"> Arbitrary nonempty string used to seed the random number generator. </param>
            SDCAOptimizer(std::shared_ptr<const DatasetType> examples, LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, std::string randomSeedString = "abc123");

            /// <summary> Constructs an instance of SDCAOptimizer (requires initialization before first use). </summary>
            ///
            /// <param name="examples"> The set of examples. </param>
            /// <param name="randomSeedString"> Arbitrary nonempty string used to seed the random number generator. </param>
            SDCAOptimizer(std::shared_ptr<const DatasetType> examples, std::string randomSeedString = "abc123");

            /// <summary> Perform one or more epochs on the examples. </summary>
            ///
            /// <param name="maxEpochs"> The maximal number of epochs to perform. </param>
            /// <param name="earlyExitDualityGap"> The desired duality gap. </param>
            void Update(size_t maxEpochs = 1, double earlyExitDualityGap = 0);

            /// <summary> Resets the SDCA optimizer. </summary>
            void Reset();

            /// <summary> Sets new parameters. </summary>
            void SetParameters(SDCAOptimizerParameters parameters);

            /// <summary> Sets a new loss function. </summary>
            void SetLossFunction(LossFunctionType lossFunction);

            /// <summary> Sets a new regularizer. </summary>
            void SetRegularizer(RegularizerType regularizer);

            /// <summary> Returns the current solution. </summary>
            const SolutionType& GetSolution() const { return _w; }

            /// <summary> Returns the current solution info. </summary>
            const SDCASolutionInfo& GetSolutionInfo() const;

        private:
            // per-example info
            struct ExampleInfo
            {
                typename SolutionType::AuxiliaryDoubleType dual;
                double norm2Squared = 0;
            };
            std::vector<ExampleInfo> _exampleInfo;

            void OneTimeSetup(std::shared_ptr<const DatasetType> examples, std::string randomSeedString);
            void InitializeDuals();
            void Step(ExampleType example, ExampleInfo& exampleInfo);

            std::shared_ptr<const DatasetType> _examples;
            LossFunctionType _lossFunction;
            RegularizerType _regularizer;
            std::default_random_engine _randomEngine;

            SolutionType _w;
            SolutionType _v;

            mutable SDCASolutionInfo _solutionInfo;
            mutable bool _areObjectivesValid = false;

            double _t = 0;
            double _lambda = 1.0;
            double _normalizedInverseLambda = 1.0;
            bool _permuteData = true;
            bool _isInitialized = false;
        };

        /// <summary> Convenience function for constructing an SDCA optimizer. </summary>
        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType> MakeSDCAOptimizer(std::shared_ptr<const typename SolutionType::DatasetType> examples, LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters);
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma region implementation

#include "Common.h"
#include "Expression.h"

#include <algorithm>
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
        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::SDCAOptimizer(std::shared_ptr<const DatasetType> examples, LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, std::string randomSeedString) :
            _lossFunction(std::move(lossFunction)),
            _regularizer(std::move(regularizer))
        {
            OneTimeSetup(examples, randomSeedString);
            SetParameters(parameters);
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::SDCAOptimizer(std::shared_ptr<const DatasetType> examples, std::string randomSeedString)
        {
            OneTimeSetup(examples, randomSeedString);
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::Update(size_t maxEpochs, double earlyExitDualityGap)
        {
            std::vector<size_t> permutation(_examples->Size());
            std::iota(permutation.begin(), permutation.end(), 0);

            // epochs
            for (size_t e = 0; e < maxEpochs; ++e)
            {
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

                _areObjectivesValid = false;
                _solutionInfo.numEpochsPerformed++;

                // early exit
                if (earlyExitDualityGap > 0)
                {
                    if (GetSolutionInfo().DualityGap() <= earlyExitDualityGap)
                    {
                        break;
                    }
                }
            }
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::Reset()
        {
            _solutionInfo.numEpochsPerformed = 0;
            _areObjectivesValid = false;

            _t = 0;
            _w.Reset();
            _v.Reset();

            InitializeDuals();
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::SetParameters(SDCAOptimizerParameters parameters)
        {
            _lambda = parameters.regularizationParameter;
            _normalizedInverseLambda = 1.0 / (_examples->Size() * parameters.regularizationParameter);
            _permuteData = parameters.permuteData;
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::SetLossFunction(LossFunctionType lossFunction)
        {
            _lossFunction = std::move(lossFunction);
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::SetRegularizer(RegularizerType regularizer)
        {
            _regularizer = std::move(regularizer);
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        const SDCASolutionInfo& SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::GetSolutionInfo() const
        {
            if (!_areObjectivesValid)
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

                _areObjectivesValid = true;
            }

            return _solutionInfo;
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::OneTimeSetup(std::shared_ptr<const DatasetType> examples, std::string randomSeedString)
        {
            // set up random engine
            std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
            _randomEngine.seed(seed);

            // check examples
            if (examples.get() == nullptr || examples->Size() == 0)
            {
                throw OptimizationException("Empty dataset");
            }

            _examples = examples;

            // resize data structures according to examples
            auto firstExample = examples->Get(0);
            _w.Resize(firstExample.input, firstExample.output);
            _v.Resize(firstExample.input, firstExample.output);
            size_t numExamples = examples->Size();
            _exampleInfo.resize(numExamples);

            // check that outputs are compatible with the loss and cache the norm2squared of each example
            for (size_t i = 0; i < numExamples; ++i)
            {
                auto example = examples->Get(i);

                if (!_lossFunction.VerifyOutput(example.output))
                {
                    throw OptimizationException("Discovered an output that is incompatible with the chosen loss function");
                }

                double norm2Squared = _w.GetNorm2SquaredOf(example.input);
                _exampleInfo[i].norm2Squared = norm2Squared;
            }

            InitializeDuals();
        }

        template <typename SolutionType, typename LossFunctionType, typename RegularizerType>
        void SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>::InitializeDuals()
        {
            for (size_t i = 0; i < _examples->Size(); ++i)
            {
                _w.InitializeAuxiliaryVariable(_exampleInfo[i].dual);
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
        SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType> MakeSDCAOptimizer(std::shared_ptr<const typename SolutionType::DatasetType> examples, LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters)
        {
            return SDCAOptimizer<SolutionType, LossFunctionType, RegularizerType>(examples, lossFunction, regularizer, parameters);
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
