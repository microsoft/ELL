////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleForestTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{
    template<typename LossFunctionType>
    SimpleForestTrainer<LossFunctionType>::SimpleForestTrainer(const LossFunctionType & lossFunction, const ForestTrainerParameters & parameters) : ForestTrainer<SplitRuleType, EdgePredictorType>(parameters), _lossFunction(lossFunction)
    {}

    template<typename LossFunctionType>
    typename SimpleForestTrainer<LossFunctionType>::SplitCandidate SimpleForestTrainer<LossFunctionType>::GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums)
    {
        auto numFeatures = _dataset.GetMaxDataVectorSize();

        SplitCandidate bestSplitCandidate(nodeId, range, sums);

        for (uint64_t inputIndex = 0; inputIndex < numFeatures; ++inputIndex)
        {
            // sort the relevant rows of dataset in ascending order by inputIndex
            SortNodeDataset(range, inputIndex);

            Sums sums0;

            // consider all thresholds
            double nextFeatureValue = _dataset[range.firstIndex].GetDataVector()[inputIndex];
            for (uint64_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size - 1; ++rowIndex)
            {
                // get friendly names
                double currentFeatureValue = nextFeatureValue;
                nextFeatureValue = _dataset[rowIndex + 1].GetDataVector()[inputIndex];

                // increment sums 
                sums0.Increment(_dataset[rowIndex].GetMetaData());

                // only split between rows with different feature values
                if (currentFeatureValue == nextFeatureValue)
                {
                    continue;
                }

                // compute sums1 and gain
                auto sums1 = sums - sums0;
                double gain = CalculateGain(sums, sums0, sums1);

                // find gain maximizer
                if (gain > bestSplitCandidate.gain)
                {
                    bestSplitCandidate.gain = gain;
                    bestSplitCandidate.splitRule = SplitRuleType{ inputIndex, 0.5 * (currentFeatureValue + nextFeatureValue) };
                    bestSplitCandidate.ranges.SetSize0(rowIndex - range.firstIndex + 1);
                    bestSplitCandidate.stats.SetChildSums({sums0, sums1});
                }
            }
        }

        return bestSplitCandidate;
    }

    template<typename LossFunctionType>
    std::vector<typename SimpleForestTrainer<LossFunctionType>::EdgePredictorType> SimpleForestTrainer<LossFunctionType>::GetEdgePredictors(const NodeStats& nodeStats)
    {
        double output = GetOutputValue(nodeStats.GetTotalSums());
        double output0 = GetOutputValue(nodeStats.GetChildSums(0)) - output;
        double output1 = GetOutputValue(nodeStats.GetChildSums(1)) - output;
        return std::vector<EdgePredictorType>{ output0, output1 };
    }

    template<typename LossFunctionType>
    void SimpleForestTrainer<LossFunctionType>::SortNodeDataset(Range range, size_t inputIndex)
    {
        _dataset.Sort([inputIndex](const ForestTrainerExample& example) { return example.GetDataVector()[inputIndex]; },
                      range.firstIndex,
                      range.size);
    }

    template<typename LossFunctionType>
    double SimpleForestTrainer<LossFunctionType>::CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const
    {
        if(sums0.sumWeights == 0 || sums1.sumWeights == 0)
        {
            return 0;
        }

        return sums0.sumWeights * _lossFunction.BregmanGenerator(sums0.sumWeightedLabels/sums0.sumWeights) +
            sums1.sumWeights * _lossFunction.BregmanGenerator(sums1.sumWeightedLabels/sums1.sumWeights) -
            sums.sumWeights * _lossFunction.BregmanGenerator(sums.sumWeightedLabels/sums.sumWeights);
    }

    template<typename LossFunctionType>
    double SimpleForestTrainer<LossFunctionType>::GetOutputValue(const Sums& sums) const
    {
        return sums.sumWeightedLabels / sums.sumWeights;
    }

    template<typename LossFunctionType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeSimpleForestTrainer(const LossFunctionType& lossFunction, const ForestTrainerParameters& parameters)
    {
        return std::make_unique<SimpleForestTrainer<LossFunctionType>>(lossFunction, parameters);
    }
}