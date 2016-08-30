////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     HistogramForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ForestTrainer.h"
#include "LogitBooster.h"
#include "ThresholdFinder.h"

// predictors
#include "SingleElementThresholdPredictor.h"
#include "ConstantPredictor.h"

// stl
#include <random>
#include <tuple>

namespace trainers
{
    /// <summary> Parameters for the forest trainer. </summary>
    struct HistogramForestTrainerParameters : public virtual ForestTrainerParameters
    {
        std::string randomSeed;
        size_t thresholdFinderSampleSize;
        size_t candidatesPerInput;
    };

    /// <summary> A histogram forest trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> The loss function type. </typeparam>
    /// <typeparam name="BoosterType"> The booster type. </typeparam>
    template <typename LossFunctionType, typename BoosterType> 
    class HistogramForestTrainer : public ForestTrainer<predictors::SingleElementThresholdPredictor, predictors::ConstantPredictor, BoosterType>
    {
    public:
        /// <summary> Constructs an instance of HistogramForestTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="booster"> The booster. </param>
        /// <param name="parameters"> Training Parameters. </param>
        HistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const HistogramForestTrainerParameters& parameters);

        using SplitRuleType = predictors::SingleElementThresholdPredictor;
        using EdgePredictorType = predictors::ConstantPredictor;
        using SplitCandidate = ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplitCandidate;

    protected:
        virtual SplitCandidate GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums) override;
        virtual std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats) override;

    private:

        struct EvaluateSplitRuleResult
        {
            Sums sums0;
            size_t size0;
        };

        double CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const;
        std::vector<SplitRuleType> GetSplitCandidatesAtNode(Range range);
        std::tuple<Sums, size_t> EvaluateSplitRule(const SplitRuleType& splitRule, const Range& range) const;

        // member variables
        LossFunctionType _lossFunction;
        std::default_random_engine _random;
        size_t _thresholdFinderSampleSize;
        size_t _candidatesPerInput;
        ExhaustiveThresholdFinder _thresholdFinder;
    };

    /// <summary> Makes a simple forest trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="lossFunction"> The loss function. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A unique_ptr to a simple forest trainer. </returns>
    template<typename LossFunctionType, typename BoosterType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeHistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const HistogramForestTrainerParameters& parameters);
}

#include "../tcc/HistogramForestTrainer.tcc"
