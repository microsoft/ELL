////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HistogramForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ForestTrainer.h"
#include "LogitBooster.h"

// predictors
#include "ConstantPredictor.h"
#include "SingleElementThresholdPredictor.h"

// stl
#include <random>
#include <tuple>

namespace ell
{
namespace trainers
{
    /// <summary> Parameters for the forest trainer. </summary>
    struct HistogramForestTrainerParameters : public virtual ForestTrainerParameters
    {
        std::string randomSeed;
        size_t thresholdFinderSampleSize;
        size_t candidatesPerInput;
    };

    /// <summary> A histogram trainer for binary decision forests with threshold split rules and constant outputs. </summary>
    ///
    /// <typeparam name="LossFunctionType"> The loss function type. </typeparam>
    /// <typeparam name="BoosterType"> The booster type. </typeparam>
    /// <typeparam name="ThresholdFinderType"> Type of the threshold finder to use. </typeparam>
    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    class HistogramForestTrainer : public ForestTrainer<predictors::SingleElementThresholdPredictor, predictors::ConstantPredictor, BoosterType>
    {
    public:
        /// <summary> Constructs an instance of HistogramForestTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="booster"> The booster. </param>
        /// <param name="thresholdFinder"> The threshold finder. </param>
        /// <param name="parameters"> Training Parameters. </param>
        HistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const ThresholdFinderType& thresholdFinder, const HistogramForestTrainerParameters& parameters);

        using SplitRuleType = predictors::SingleElementThresholdPredictor;
        using EdgePredictorType = predictors::ConstantPredictor;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplitCandidate;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplittableNodeId;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeStats;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Range;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Sums;

    protected:
        using ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::_dataset;
        SplitCandidate GetBestSplitRuleAtNode(SplittableNodeId nodeId, Range range, Sums sums) override;
        std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats) override;

    private:
        struct EvaluateSplitRuleResult
        {
            Sums sums0;
            size_t size0;
        };

        double CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const;
        std::vector<SplitRuleType> CallThresholdFinder(Range range);
        std::tuple<Sums, size_t> EvaluateSplitRule(const SplitRuleType& splitRule, const Range& range) const;

        // member variables
        LossFunctionType _lossFunction;
        ThresholdFinderType _thresholdFinder;
        std::default_random_engine _random;
        size_t _thresholdFinderSampleSize;
        size_t _candidatesPerInput;
    };

    /// <summary> Makes a simple forest trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="lossFunction"> The loss function. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A unique_ptr to a simple forest trainer. </returns>
    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    std::unique_ptr<ITrainer<predictors::SimpleForestPredictor>> MakeHistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const ThresholdFinderType& thresholdFinder, const HistogramForestTrainerParameters& parameters);
}
}

#include "../tcc/HistogramForestTrainer.tcc"
