////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     HistogramForestTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "RandomEngines.h"

namespace trainers
{
    template<typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::HistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const ThresholdFinderType& thresholdFinder, const HistogramForestTrainerParameters& parameters) 
        : ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>(booster, parameters), _lossFunction(lossFunction), _thresholdFinder(thresholdFinder), _random(utilities::GetRandomEngine(parameters.randomSeed)), _candidatesPerInput(parameters.candidatesPerInput), _thresholdFinderSampleSize(parameters.thresholdFinderSampleSize)
    {}

    template<typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    typename HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::SplitCandidate HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums)
    {
        SplitCandidate bestSplitCandidate(nodeId, range, sums);

        auto splitRuleCandidates = GetSplitCandidatesAtNode(range);

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
                bestSplitCandidate.ranges.SetSize0(size0);
                bestSplitCandidate.stats.SetChildSums({sums0, sums1});
            }
        }

        return bestSplitCandidate;
    }

    template<typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    std::vector<typename HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::EdgePredictorType> HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::GetEdgePredictors(const NodeStats& nodeStats)
    {
        double output = nodeStats.GetTotalSums().GetMeanLabel();
        double output0 = nodeStats.GetChildSums(0).GetMeanLabel() - output;
        double output1 = nodeStats.GetChildSums(1).GetMeanLabel() - output;
        return std::vector<EdgePredictorType>{ output0, output1 };
    }

    template<typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    double HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const
    {
        if(sums0.sumWeights == 0 || sums1.sumWeights == 0)
        {
            return 0;
        }

        return sums0.sumWeights * _lossFunction.BregmanGenerator(sums0.sumWeightedLabels/sums0.sumWeights) +
            sums1.sumWeights * _lossFunction.BregmanGenerator(sums1.sumWeightedLabels/sums1.sumWeights) -
            sums.sumWeights * _lossFunction.BregmanGenerator(sums.sumWeightedLabels/sums.sumWeights);
    }

    template<typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    std::vector<typename HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::SplitRuleType> HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::GetSplitCandidatesAtNode(Range range)
    {
        // uniformly choose _candidatesPerInput from the range, without replacement
        _dataset.RandomPermute(_random, range.firstIndex, range.size, _thresholdFinderSampleSize);

        auto exampleIterator = _dataset.GetIterator(range.firstIndex, _thresholdFinderSampleSize);
        auto thresholds = _thresholdFinder.GetThresholds(exampleIterator);
        return thresholds;
    }

    template<typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    std::tuple<ForestTrainerBase::Sums, size_t> HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>::EvaluateSplitRule(const SplitRuleType& splitRule, const Range& range) const
    {
        Sums sums0;
        size_t size0 = 0;

        auto exampleIterator = _dataset.GetIterator(range.firstIndex, range.size);
        while(exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();
            auto prediction = splitRule.Predict(example.GetDataVector());
            if(prediction == 0)
            {
                sums0.Increment(example.GetMetadata().weak);
                ++size0;
            }
            exampleIterator.Next();
        }

        return std::make_tuple(sums0, size0);
    };

    template<typename LossFunctionType, typename BoosterType, typename ThresholdFinderType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeHistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const ThresholdFinderType& thresholdFinder, const HistogramForestTrainerParameters& parameters)
    {
        return std::make_unique<HistogramForestTrainer<LossFunctionType, BoosterType, ThresholdFinderType>>(lossFunction, booster, thresholdFinder, parameters);
    }
}