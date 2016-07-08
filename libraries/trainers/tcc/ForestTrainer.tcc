////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//#define VERY_VERBOSE

namespace trainers
{    
    template <typename LossFunctionType>
    ForestTrainer<LossFunctionType>::ForestTrainer(const LossFunctionType& lossFunction, const ForestTrainerParameters& parameters) :
        _lossFunction(lossFunction), _parameters(parameters), _forest(std::make_shared<predictors::SimpleForestPredictor>())
    {}

    template <typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::Update(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        // convert data fron iterator to dense row dataset; compute sums statistics of the tree root
        auto sums = LoadData(exampleIterator);

        // find split candidate for root node and push it onto the priority queue
        AddSplitCandidateToQueue(_forest->GetRootId(), 0, _dataset.NumExamples(), sums);

        // as long as positive gains can be attained, keep growing the tree
        while(!_queue.empty())
        {

#ifdef VERY_VERBOSE
             std::cout << "Iteration\n";
             _queue.Print(std::cout, _dataset);
#endif

            auto splitCandidate = _queue.top();
            _queue.pop();

            const auto& sums = splitCandidate.nodeStats.sums;
            const auto& sums0 = splitCandidate.nodeStats.sums0;
            auto sums1 = sums - sums0;

            // perform the split
            double outputValueSoFar = GetOutputValue(sums);
            double negativeOutputValue = GetOutputValue(sums0) - outputValueSoFar;
            double positiveOutputValue = GetOutputValue(sums1) - outputValueSoFar;
            auto interiorNodeIndex = _forest->Split(splitCandidate.splitInfo, splitCandidate.nodeId);

            // sort the data according to the performed split
            SortDatasetByFeature(splitCandidate.featureIndex, splitCandidate.nodeStats.fromRowIndex, splitCandidate.nodeStats.size);

            // queue split candidate for child 0
            AddSplitCandidateToQueue(_forest->GetChildId(interiorNodeIndex, 0), splitCandidate.nodeStats.fromRowIndex, splitCandidate.nodeStats.size0, sums0);

            // queue split candidate for child 1
            AddSplitCandidateToQueue(_forest->GetChildId(interiorNodeIndex, 1), splitCandidate.nodeStats.fromRowIndex + splitCandidate.nodeStats.size0, splitCandidate.nodeStats.size - splitCandidate.nodeStats.size0, sums1);
        }
    }

    template<typename LossFunctionType>
    typename ForestTrainer<LossFunctionType>::Sums ForestTrainer<LossFunctionType>::Sums::operator-(const typename ForestTrainer<LossFunctionType>::Sums& other) const
    {
        Sums difference;
        difference.sumWeights = sumWeights - other.sumWeights;
        difference.sumWeightedLabels = sumWeightedLabels - other.sumWeightedLabels;
        return difference;
    }

    template<typename LossFunctionType>
    typename ForestTrainer<LossFunctionType>::Sums ForestTrainer<LossFunctionType>::LoadData(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        Sums sums;

        // create DenseRowDataset: TODO this code breaks encapsulation
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();
            sums.sumWeights += example.GetWeight();
            sums.sumWeightedLabels += example.GetWeight() * example.GetLabel();
            auto denseDataVector = std::make_unique<dataset::DoubleDataVector>(example.GetDataVector().ToArray());
            auto denseSupervisedExample = dataset::SupervisedExample<dataset::DoubleDataVector>(std::move(denseDataVector), example.GetLabel(), example.GetWeight());
            _dataset.AddExample(std::move(denseSupervisedExample));
            exampleIterator.Next();
        }

        return sums;
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::AddSplitCandidateToQueue(SplittableNodeId nodeId, uint64_t fromRowIndex, uint64_t size, Sums sums)
    {
        auto numFeatures = _dataset.GetMaxDataVectorSize();

        double bestGain = 0;
        size_t bestFeatureIndex;
        double bestThreshold;
        NodeStats bestNodeStats;

        for (uint64_t featureIndex = 0; featureIndex < numFeatures; ++featureIndex)
        {
            // sort the relevant rows of dataset in ascending order by featureIndex
            SortDatasetByFeature(featureIndex, fromRowIndex, size);

            Sums sums0;

            // consider all thresholds
            for (uint64_t rowIndex = fromRowIndex; rowIndex < fromRowIndex + size-1; ++rowIndex)
            {
                // get friendly names
                const auto& currentRow = _dataset[rowIndex].GetDataVector();
                const auto& nextRow = _dataset[rowIndex+1].GetDataVector();
                double weight = _dataset[rowIndex].GetWeight();
                double label = _dataset[rowIndex].GetLabel();

                // increment sums
                sums0.sumWeights += weight;
                sums0.sumWeightedLabels += weight * label;

                // only split between rows with different feature values
                if (currentRow[featureIndex] == nextRow[featureIndex])
                {
                    continue;
                }

                // find gain maximizer
                double gain = CalculateGain(sums, sums0);
                if (gain > bestGain)
                {
                    bestGain = gain;
                    bestFeatureIndex = featureIndex;
                    bestThreshold = 0.5 * (currentRow[featureIndex] + nextRow[featureIndex]);
                    
                    bestNodeStats.size = size;
                    bestNodeStats.size0 = rowIndex + 1 - fromRowIndex;
                    bestNodeStats.sums = sums;
                    bestNodeStats.sums0 = sums0;
                }
            }
        }

        using SplitRule = predictors::SingleInputThresholdRule;
        using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;

        // if found a good split candidate, queue it
        if (bestGain > 0.0)
        {
            SplitInfo splitInfo{SplitRule{ bestFeatureIndex, bestThreshold }, EdgePredictorVector{ GetOutputValue(bestNodeStats.sums0), GetOutputValue(bestNodeStats.sums0) }}; // TODO
            SplitCandidate splitCandidate{splitInfo, nodeId, bestNodeStats, bestGain, bestFeatureIndex, bestThreshold};
            _queue.push(std::move(splitCandidate));
        }

#ifdef VERY_VERBOSE
        else
        {
            std::cout << "No positive-gain split candidate found - queue unmodified\n";
            splitCandidate.Print(std::cout, _dataset);
        }
#endif
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::SortDatasetByFeature(uint64_t featureIndex, uint64_t fromRowIndex, uint64_t size)
    {
        _dataset.Sort([featureIndex](const dataset::SupervisedExample<dataset::DoubleDataVector>& example) {return example.GetDataVector()[featureIndex]; },
            fromRowIndex,
            size);
    }

    template<typename LossFunctionType>
    double ForestTrainer<LossFunctionType>::CalculateGain(Sums sums, Sums sums0) const
    {
        auto positiveSums = sums - sums0;

        if(sums0.sumWeights == 0 || positiveSums.sumWeights == 0)
        {
            return 0;
        }
        
        return sums0.sumWeights * _lossFunction.BregmanGenerator(sums0.sumWeightedLabels/sums0.sumWeights) +
            positiveSums.sumWeights * _lossFunction.BregmanGenerator(positiveSums.sumWeightedLabels/positiveSums.sumWeights) -
            sums.sumWeights * _lossFunction.BregmanGenerator(sums.sumWeightedLabels/sums.sumWeights);
    }

    template<typename LossFunctionType>
    double ForestTrainer<LossFunctionType>::GetOutputValue(Sums sums) const
    {
        return sums.sumWeightedLabels / sums.sumWeights;
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::SplitCandidate::Print(std::ostream& os, const dataset::RowDataset<dataset::DoubleDataVector>& dataset) const
    {
        os << "Leaf: " << leaf <<
            "\tSplitRule: (" << splitRule.featureIndex << "," << splitRule.threshold << ")" <<
            "\tGain: " << gain <<
            "\tSize: " << size <<
            "\tNegativeSize: " << size0 <<
            "\tSums: (" << sums.sumWeights << "," << sums.sumWeightedLabels << ")" <<
            "\tNegativeSums: (" << sums0.sumWeights << "," << sums0.sumWeightedLabels << ")\n";

        dataset.Print(os, fromRowIndex, size);
        os << std::endl;
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::PriorityQueue::Print(std::ostream& os, const dataset::RowDataset<dataset::DoubleDataVector>& dataset) const
    {
        os << "Priority Queue Size: " << size() << "\n";

        // TODO: use the heapify routines on a normal vector if we want to iterate over the items
        for(const auto& candidate : std::priority_queue<SplitCandidate>::c) // c is a protected member of std::priority_queue
        {
            candidate.Print(os, dataset);
        }
    }

    template<typename LossFunctionType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeForestTrainer(const LossFunctionType& lossFunction, const ForestTrainerParameters& parameters)
    {
        return std::make_unique<ForestTrainer<LossFunctionType>>(lossFunction, parameters);
    }
}
