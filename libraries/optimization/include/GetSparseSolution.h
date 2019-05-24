////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GetSparseSolution.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BinarySearch.h"
#include "ElasticNetRegularizer.h"
#include "ExponentialSearch.h"
#include "SDCAOptimizer.h"

#include <memory>

namespace ell
{
namespace optimization
{
    /// <summary> Parameters for the GetSparseSolution procedure. </summary>
    struct GetSparseSolutionParameters
    {
        Interval targetDensity; // the target density, an interval contained in [0, 1]

        // main SDCA parameters
        SDCAOptimizerParameters SDCAParameters = { 0.001 }; // parameters to the SDCA optimizer

        // budget of allowed epochs
        size_t maxEpochs = 200; // the total number of SDCA epochs, across exponential search and binary search

        // Other SDCA parameters
        size_t SDCAMaxEpochsPerCall = 20; // Perform at most this many SDCA epochs each time it invokes SDCA
        double SDCAEarlyExitDualityGap = 1.0e-3; // stop SDCA early if this duality gap is reached
        std::string SDCARandomSeedString = "abc123"; // random seed

        // Exponential search parameters
        double exponentialSearchGuess = 1.0; // a first guess of the L1 regularization parameter that would give the desired result
        double exponentialSearchBase = 2.0; // base of the exponential search, must be greater than 1.0; 2.0 means that the interval doubles with each iteration
    };

    template <typename SolutionType>
    struct SparseSolution
    {
        SolutionType solution;
        SDCASolutionInfo info;
        double beta;
        double density;
    };

    /// <summary> Returns a solution with a specified level of density/sparsity. </summary>
    /// <param name="examples"> A shared pointer to a set of examples. </param>
    /// <param name="lossFunction"> A loss function. </param>
    /// <param name="parameters"> An instance of GetSparseSolutionParameters. </param>
    template <typename SolutionType, typename LossFunctionType>
    SparseSolution<SolutionType> GetSparseSolution(std::shared_ptr<const typename SolutionType::DatasetType> examples, LossFunctionType lossFunction, GetSparseSolutionParameters parameters);
} // namespace optimization
} // namespace ell

#pragma region implementation

namespace ell
{
namespace optimization
{
    template <typename SolutionType, typename LossFunctionType>
    SparseSolution<SolutionType> GetSparseSolution(std::shared_ptr<const typename SolutionType::DatasetType> examples, LossFunctionType lossFunction, GetSparseSolutionParameters parameters)
    {
        // create optimizer
        auto optimizer = MakeSDCAOptimizer<SolutionType>(examples, lossFunction, ElasticNetRegularizer{}, parameters.SDCAParameters, parameters.SDCARandomSeedString);

        // SDCA epoch budget (leave some for the final call to the optimizer)
        size_t epochBudget = parameters.maxEpochs - parameters.SDCAMaxEpochsPerCall;

        // create a function that monotonically maps [-infinity,infinity] to the fraction of non-zeros
        auto getDensity = [&](double minusLogScale) {
            // compute the L1 regularization parameter beta (which is later multiplied by the SDCA regularization parameter)
            double beta = parameters.exponentialSearchGuess * std::exp(-minusLogScale);

            // optimize
            optimizer.SetRegularizer(ElasticNetRegularizer{ beta });
            size_t numEpochs = std::min(epochBudget, parameters.SDCAMaxEpochsPerCall);
            optimizer.Update(numEpochs, parameters.SDCAEarlyExitDualityGap);

            // post-process
            epochBudget -= optimizer.GetSolutionInfo().numEpochsPerformed;
            const auto& vector = optimizer.GetSolution().GetVector();
            double density = vector.Norm0() / vector.Size();
            optimizer.Reset();
            return density;
        };

        // use exponential search to find an upper bound and lower bound on the minusLogScale parameter
        ExponentialSearchParameters boundarySearchParameters{ parameters.targetDensity, parameters.exponentialSearchGuess, parameters.exponentialSearchBase };
        auto exponentialSearch = ExponentialSearch(getDensity, boundarySearchParameters);
        while (!exponentialSearch.IsSuccessful() && epochBudget > parameters.SDCAMaxEpochsPerCall)
        {
            exponentialSearch.Update();
        }

        // if we were lucky, exponential search already found a good value of minusLogScale
        double bestMinusLogScale = 0;
        if (exponentialSearch.GetBoundingArguments().Size() == 0)
        {
            bestMinusLogScale = exponentialSearch.GetBoundingArguments().Begin();
        }

        // otherwise, use binary search to zero in on a good value of minusLogScale
        else
        {
            BinarySearchParameters binarySearchParameters{ parameters.targetDensity, exponentialSearch.GetBoundingArguments(), true, exponentialSearch.GetBoundingValues() };
            auto binarySearch = BinarySearch(getDensity, binarySearchParameters);
            while (!binarySearch.IsSuccessful() && epochBudget > parameters.SDCAMaxEpochsPerCall)
            {
                binarySearch.Update();
            }

            bestMinusLogScale = binarySearch.GetCurrentSearchInterval().Begin();
        }

        // retrain solution
        double bestBeta = parameters.exponentialSearchGuess * std::exp(-bestMinusLogScale);
        optimizer.SetRegularizer(ElasticNetRegularizer{ bestBeta });
        optimizer.Update(parameters.SDCAMaxEpochsPerCall, parameters.SDCAEarlyExitDualityGap);

        const auto& vector = optimizer.GetSolution().GetVector();
        double density = vector.Norm0() / vector.Size();
        return { optimizer.GetSolution(), optimizer.GetSolutionInfo(), bestBeta, density };
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
