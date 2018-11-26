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

#include <predictors/include/ConstantPredictor.h>
#include <predictors/include/SingleElementThresholdPredictor.h>

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
} // namespace trainers
} // namespace ell

#pragma region implementation

#include <utilities/include/RandomEngines.h>

namespace ell
{
namespace trainers
{
    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::HistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const ThresholdFinderType& thresholdFinder, const HistogramForestTrainerParameters& parameters) :
        ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>(booster, parameters),
        _lossFunction(lossFunction),
        _thresholdFinder(thresholdFinder),
        _random(utilities::GetRandomEngine(parameters.randomSeed)),
        _thresholdFinderSampleSize(parameters.thresholdFinderSampleSize),
        _candidatesPerInput(parameters.candidatesPerInput)
    {
    }

    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    auto HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::GetBestSplitRuleAtNode(SplittableNodeId nodeId, Range range, Sums sums) -> SplitCandidate
    {
        SplitCandidate bestSplitCandidate(nodeId, range, sums);

        auto splitRuleCandidates = CallThresholdFinder(range);

        for (const auto& splitRuleCandidate : splitRuleCandidates)
        {
            Sums sums0;
            size_t size0;

            std::tie(sums0, size0) = EvaluateSplitRule(splitRuleCandidate, range);

            Sums sums1 = sums - sums0;
            double gain = CalculateGain(sums, sums0, sums1);

            // find gain maximizer
            if (gain > bestSplitCandidate.gain)
            {
                bestSplitCandidate.gain = gain;
                bestSplitCandidate.splitRule = splitRuleCandidate;
                bestSplitCandidate.ranges.SplitChildRange(0, size0);
                bestSplitCandidate.stats.SetChildSums({ sums0, sums1 });
            }
        }

        return bestSplitCandidate;
    }

    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    auto HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::GetEdgePredictors(const NodeStats& nodeStats) -> std::vector<EdgePredictorType>
    {
        double output = nodeStats.GetTotalSums().GetMeanLabel();
        double output0 = nodeStats.GetChildSums(0).GetMeanLabel() - output;
        double output1 = nodeStats.GetChildSums(1).GetMeanLabel() - output;
        return std::vector<EdgePredictorType>{ output0, output1 };
    }

    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    double HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const
    {
        if (sums0.sumWeights == 0 || sums1.sumWeights == 0)
        {
            return 0;
        }

        return sums0.sumWeights * _lossFunction.BregmanGenerator(sums0.sumWeightedLabels / sums0.sumWeights) +
               sums1.sumWeights * _lossFunction.BregmanGenerator(sums1.sumWeightedLabels / sums1.sumWeights) -
               sums.sumWeights * _lossFunction.BregmanGenerator(sums.sumWeightedLabels / sums.sumWeights);
    }

    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    auto HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::CallThresholdFinder(Range range) -> std::vector<SplitRuleType>
    {
        // uniformly choose _candidatesPerInput from the range, without replacement
        _dataset.RandomPermute(_random, range.firstIndex, range.size, _thresholdFinderSampleSize);

        auto thresholds = _thresholdFinder.GetThresholds(_dataset.GetExampleReferenceIterator(range.firstIndex, _thresholdFinderSampleSize));
        return thresholds;
    }

    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    auto HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::EvaluateSplitRule(const SplitRuleType& splitRule, const Range& range) const -> std::tuple<Sums, size_t>
    {
        Sums sums0;
        size_t size0 = 0;

        auto exampleIterator = _dataset.GetExampleIterator(range.firstIndex, range.size);
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();
            auto prediction = splitRule.Predict(example.GetDataVector());
            if (prediction == 0)
            {
                sums0.Increment(example.GetMetadata().weak);
                ++size0;
            }
            exampleIterator.Next();
        }

        return std::make_tuple(sums0, size0);
    };

    template <typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    std::unique_ptr<ITrainer<predictors::SimpleForestPredictor>> MakeHistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const ThresholdFinderType& thresholdFinder, const HistogramForestTrainerParameters& parameters)
    {
        return std::make_unique<HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>>(lossFunction, booster, thresholdFinder, parameters);
    }
} // namespace trainers
} // namespace ell

#pragma endregion implementation
