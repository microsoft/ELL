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
    template<typename LossFunctionType, typename BoosterType>
    HistogramForestTrainer<LossFunctionType, BoosterType>::HistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const HistogramForestTrainerParameters& parameters) 
        : ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>(booster, parameters), _lossFunction(lossFunction), _random(utilities::GetRandomEngine(parameters.randomSeed)), _candidatesPerInput(parameters.candidatesPerInput), _thresholdFinderSampleSize(parameters.thresholdFinderSampleSize)
    {}

    template<typename LossFunctionType, typename BoosterType>
    typename HistogramForestTrainer<LossFunctionType, BoosterType>::SplitCandidate HistogramForestTrainer<LossFunctionType, BoosterType>::GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums)
    {
        SplitCandidate bestSplitCandidate(nodeId, range, sums);

        auto splitRuleCandidates = GetSplitCandidatesAtNode(range);

        for (const auto splitRuleCandidate : splitRuleCandidates) 
        {
            // get sums0 and sums1

            Sums sums0;
            Sums sums1;
            size_t size0;
//            sums0.Increment(_dataset[rowIndex].GetMetaData().weak);
//              auto sums1 = sums - sums0;
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

    template<typename LossFunctionType, typename BoosterType>
    std::vector<typename HistogramForestTrainer<LossFunctionType, BoosterType>::EdgePredictorType> HistogramForestTrainer<LossFunctionType, BoosterType>::GetEdgePredictors(const NodeStats& nodeStats)
    {
        double output = nodeStats.GetTotalSums().GetMeanLabel();
        double output0 = nodeStats.GetChildSums(0).GetMeanLabel() - output;
        double output1 = nodeStats.GetChildSums(1).GetMeanLabel() - output;
        return std::vector<EdgePredictorType>{ output0, output1 };
    }

    template<typename LossFunctionType, typename BoosterType>
    double HistogramForestTrainer<LossFunctionType, BoosterType>::CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const
    {
        if(sums0.sumWeights == 0 || sums1.sumWeights == 0)
        {
            return 0;
        }

        return sums0.sumWeights * _lossFunction.BregmanGenerator(sums0.sumWeightedLabels/sums0.sumWeights) +
            sums1.sumWeights * _lossFunction.BregmanGenerator(sums1.sumWeightedLabels/sums1.sumWeights) -
            sums.sumWeights * _lossFunction.BregmanGenerator(sums.sumWeightedLabels/sums.sumWeights);
    }

    template<typename LossFunctionType, typename BoosterType>
    std::vector<typename HistogramForestTrainer<LossFunctionType, BoosterType>::SplitRuleType> HistogramForestTrainer<LossFunctionType, BoosterType>::GetSplitCandidatesAtNode(Range range)
    {
        // uniformly choose _candidatesPerInput from the range, without replacement
        _dataset.RandomPermute(_random, range.firstIndex, range.size, _thresholdFinderSampleSize);

        auto exampleIterator = _dataset.GetIterator(range.firstIndex, _thresholdFinderSampleSize);
        auto thresholds = _thresholdFinder.GetThresholds(exampleIterator);
        return thresholds;
    }

    template<typename LossFunctionType, typename BoosterType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeHistogramForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const HistogramForestTrainerParameters& parameters)
    {
        return std::make_unique<HistogramForestTrainer<LossFunctionType, BoosterType>>(lossFunction, booster, parameters);
    }
}