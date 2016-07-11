////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingTreeTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//#define VERY_VERBOSE

namespace trainers
{    
    template <typename LossFunctionType>
    SortingTreeTrainer<LossFunctionType>::SortingTreeTrainer(const LossFunctionType& lossFunction, const SortingTreeTrainerParameters& parameters) :
        _lossFunction(lossFunction), _parameters(parameters)
    {}

    template <typename LossFunctionType>
    predictors::DecisionTreePredictor SortingTreeTrainer<LossFunctionType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        // convert data fron iterator to dense row dataset; compute sums statistics of the tree root
        auto sums = LoadData(exampleIterator);

        predictors::DecisionTreePredictor tree(GetOutputValue(sums));

        // find split candidate for root node and push it onto the priority queue
        AddSplitCandidateToQueue(&tree.GetRoot(), 0, _dataset.NumExamples(), sums);

        // as long as positive gains can be attained, keep growing the tree
        while(!_queue.empty())
        {

#ifdef VERY_VERBOSE
             std::cout << "Iteration\n";
             _queue.Print(std::cout, _dataset);
#endif

            auto splitInfo = _queue.top();
            _queue.pop();

            auto positiveSums = splitInfo.sums - splitInfo.sums0;

            // perform the split
            double outputValueSoFar = GetOutputValue(splitInfo.sums);
            double negativeOutputValue = GetOutputValue(splitInfo.sums0) - outputValueSoFar;
            double positiveOutputValue = GetOutputValue(positiveSums) - outputValueSoFar;
            auto& interiorNode = splitInfo.leaf->Split(splitInfo.splitRule, negativeOutputValue, positiveOutputValue);

            // sort the data according to the performed split
            SortDatasetBySplitRule(splitInfo.splitRule.featureIndex, splitInfo.fromRowIndex, splitInfo.size);

            // queue split candidate for negative child
            AddSplitCandidateToQueue(&interiorNode.GetNegativeChild(), splitInfo.fromRowIndex, splitInfo.size0, splitInfo.sums0);

            // queue split candidate for positive child
            AddSplitCandidateToQueue(&interiorNode.GetPositiveChild(), splitInfo.fromRowIndex + splitInfo.size0, splitInfo.size - splitInfo.size0, positiveSums);
        }

        Cleanup();

        return tree;
    }

    template<typename LossFunctionType>
    typename SortingTreeTrainer<LossFunctionType>::Sums SortingTreeTrainer<LossFunctionType>::Sums::operator-(const typename SortingTreeTrainer<LossFunctionType>::Sums& other) const
    {
        Sums difference;
        difference.sumWeights = sumWeights - other.sumWeights;
        difference.sumWeightedLabels = sumWeightedLabels - other.sumWeightedLabels;
        return difference;
    }

    template<typename LossFunctionType>
    typename SortingTreeTrainer<LossFunctionType>::Sums SortingTreeTrainer<LossFunctionType>::LoadData(dataset::GenericRowDataset::Iterator exampleIterator) const
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
    void SortingTreeTrainer<LossFunctionType>::AddSplitCandidateToQueue(predictors::DecisionTreePredictor::Node* leaf, uint64_t fromRowIndex, uint64_t size, Sums sums) const
    {
        auto numFeatures = _dataset.GetMaxDataVectorSize();

        SplitCandidate splitCandidate;
        splitCandidate.leaf = leaf;
        splitCandidate.fromRowIndex = fromRowIndex;
        splitCandidate.size = size;

        for (uint64_t featureIndex = 0; featureIndex < numFeatures; ++featureIndex)
        {
            // sort the relevant rows of dataset in ascending order by featureIndex
            SortDatasetBySplitRule(featureIndex, fromRowIndex, size);

            Sums sums0;

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
                if (gain > splitCandidate.gain)
                {
                    splitCandidate.gain = gain;
                    splitCandidate.splitRule.featureIndex = featureIndex;
                    splitCandidate.splitRule.threshold = 0.5 * (currentRow[featureIndex] + nextRow[featureIndex]);
                    splitCandidate.size0 = rowIndex + 1 - fromRowIndex;
                    splitCandidate.sums = sums;
                    splitCandidate.sums0 = sums0;
                }
            }
        }

        // if found a good split candidate, queue it
        if (splitCandidate.gain > 0.0)
        {
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
    void SortingTreeTrainer<LossFunctionType>::SortDatasetBySplitRule(uint64_t featureIndex, uint64_t fromRowIndex, uint64_t size) const
    {
        _dataset.Sort([featureIndex](const dataset::SupervisedExample<dataset::DoubleDataVector>& example) {return example.GetDataVector()[featureIndex]; },
            fromRowIndex,
            size);
    }

    template<typename LossFunctionType>
    double SortingTreeTrainer<LossFunctionType>::CalculateGain(Sums sums, Sums sums0) const
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
    double SortingTreeTrainer<LossFunctionType>::GetOutputValue(Sums sums) const
    {
        return sums.sumWeightedLabels / sums.sumWeights;
    }

    template<typename LossFunctionType>
    void SortingTreeTrainer<LossFunctionType>::Cleanup() const
    {
        _dataset.Reset();
        while (!_queue.empty())
        {
            _queue.pop();
        }
    }

    template<typename LossFunctionType>
    std::unique_ptr<IBlackBoxTrainer<predictors::DecisionTreePredictor>> MakeSortingTreeTrainer(const LossFunctionType& lossFunction, const SortingTreeTrainerParameters& parameters)
    {
        return std::make_unique<SortingTreeTrainer<LossFunctionType>>(lossFunction, parameters);
    }

    template<typename LossFunctionType>
    void SortingTreeTrainer<LossFunctionType>::SplitCandidate::Print(std::ostream& os, const dataset::RowDataset<dataset::DoubleDataVector>& dataset) const
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
    void SortingTreeTrainer<LossFunctionType>::PriorityQueue::Print(std::ostream& os, const dataset::RowDataset<dataset::DoubleDataVector>& dataset) const
    {
        os << "Priority Queue Size: " << size() << "\n";

        // TODO: use the heapify routines on a normal vector if we want to iterate over the items
        for(const auto& candidate : std::priority_queue<SplitCandidate>::c) // c is a protected member of std::priority_queue
        {
            candidate.Print(os, dataset);
        }
    }
}
