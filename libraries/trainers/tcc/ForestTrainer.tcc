////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#define VERY_VERBOSE

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
        AddSplitCandidateToQueue(_forest->GetNewRootId(), 0, _dataset.NumExamples(), sums);

        // as long as positive gains can be attained, keep growing the tree
        while(!_queue.empty())
        {

#ifdef VERY_VERBOSE
             std::cout << "Iteration\n";
             _queue.Print(std::cout);
#endif

            auto splitCandidate = _queue.top();
            _queue.pop();

            const auto& sums = splitCandidate.nodeStats.sums;
            const auto& sums0 = splitCandidate.nodeStats.sums0;
            const auto& sums1 = splitCandidate.nodeStats.sums1;
            auto size = splitCandidate.nodeExamples.size;
            auto size0 = splitCandidate.nodeExamples.size0;
            auto size1 = splitCandidate.nodeExamples.size1;
            auto fromRowIndex0 = splitCandidate.nodeExamples.fromRowIndex;
            auto fromRowIndex1 = fromRowIndex0 + size0;

            // perform the split
            double outputValueSoFar = GetOutputValue(sums); // TODO - root split creates a bug, send this as function argument
            double negativeOutputValue = GetOutputValue(sums0) - outputValueSoFar;
            double positiveOutputValue = GetOutputValue(sums1) - outputValueSoFar;
            auto interiorNodeIndex = _forest->Split(splitCandidate.splitAction);

            // sort the data according to the performed split
            SortDatasetBySplitRule(splitCandidate.splitAction.GetSplitRule(), splitCandidate.nodeExamples.fromRowIndex, size);

            // queue split candidate for child 0
            AddSplitCandidateToQueue(_forest->GetChildId(interiorNodeIndex, 0), fromRowIndex0, size0, std::move(sums0));

            // queue split candidate for child 1
            AddSplitCandidateToQueue(_forest->GetChildId(interiorNodeIndex, 1), fromRowIndex1, size1, std::move(sums1));
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
            
            sums.Increment(example.GetMetaData().GetWeight(), example.GetMetaData().GetLabel()); // TODO - maybe this func should just take a WeightLabel?

            auto denseDataVector = std::make_unique<dataset::DoubleDataVector>(example.GetDataVector().ToArray());
            auto denseSupervisedExample = ForestTrainerExample(std::move(denseDataVector), example.GetMetaData());
            _dataset.AddExample(std::move(denseSupervisedExample));
            exampleIterator.Next();
        }

        return sums;
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::AddSplitCandidateToQueue(SplittableNodeId nodeId, uint64_t fromRowIndex, size_t size, Sums sums)
    {
        auto numFeatures = _dataset.GetMaxDataVectorSize();

        double bestGain = 0;
        size_t bestFeatureIndex;
        double bestThreshold;
        size_t bestSize0;
        
        NodeStats bestNodeStats;
        bestNodeStats.sums = sums;

        for (uint64_t featureIndex = 0; featureIndex < numFeatures; ++featureIndex)
        {
            // sort the relevant rows of dataset in ascending order by featureIndex
            SortDatasetBySplitRule(featureIndex, fromRowIndex, size);

            Sums sums0;

            // consider all thresholds
            for (uint64_t rowIndex = fromRowIndex; rowIndex < fromRowIndex + size-1; ++rowIndex)
            {
                // get friendly names
                const auto& currentRow = _dataset[rowIndex].GetDataVector();
                const auto& nextRow = _dataset[rowIndex+1].GetDataVector();
                double weight = _dataset[rowIndex].GetMetaData().GetWeight();
                double label = _dataset[rowIndex].GetMetaData().GetLabel();

                // increment sums 
                sums0.Increment(weight, label); 

                // only split between rows with different feature values
                if (currentRow[featureIndex] == nextRow[featureIndex])
                {
                    continue;
                }

                // compute sums1
                auto sums1 = sums - sums0;

                // find gain maximizer
                double gain = CalculateGain(sums, sums0, sums1);
                if (gain > bestGain)
                {
                    bestGain = gain;
                    bestFeatureIndex = featureIndex;
                    bestThreshold = 0.5 * (currentRow[featureIndex] + nextRow[featureIndex]);
                    bestSize0 = rowIndex - fromRowIndex + 1;
                    bestNodeStats.sums0 = sums0;
                    bestNodeStats.sums1 = sums1; 
                }
            }
        }

        // if found a good split candidate, queue it
        if (bestGain > 0.0)
        {
            using SplitRule = predictors::SingleInputThresholdRule;
            using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;

            EdgePredictorVector edgePredictorVector{ GetOutputValue(bestNodeStats.sums0) , GetOutputValue(bestNodeStats.sums1) };
            SplitAction splitAction(nodeId, SplitRule{ bestFeatureIndex, bestThreshold }, std::move(edgePredictorVector));
            NodeExamples nodeExamples{ fromRowIndex, size, bestSize0, size - bestSize0 };
            _queue.push(SplitCandidate{ std::move(splitAction), std::move(bestNodeStats), std::move(nodeExamples), bestGain });
        }

#ifdef VERY_VERBOSE
        else
        {
            std::cout << "No positive-gain split candidate found - queue unmodified\n";
        }
#endif
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::SortDatasetBySplitRule(size_t featureIndex, uint64_t fromRowIndex, uint64_t size) // to be deprecated
    {
        _dataset.Sort([featureIndex](const ForestTrainerExample& example) { return example.GetDataVector()[featureIndex]; },
                      fromRowIndex,
                      size);
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::SortDatasetBySplitRule(const SplitRuleType& splitRule, uint64_t fromRowIndex, uint64_t size)
    {
        _dataset.Sort([splitRule](const ForestTrainerExample& example) { return splitRule.Compute(example.GetDataVector()); },
                      fromRowIndex,
                      size);
    }

    template<typename LossFunctionType>
    double ForestTrainer<LossFunctionType>::CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const
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
    double ForestTrainer<LossFunctionType>::GetOutputValue(const Sums& sums) const
    {
        return sums.sumWeightedLabels / sums.sumWeights;
    }

    template<typename LossFunctionType>
    void trainers::ForestTrainer<LossFunctionType>::Sums::Increment(double weight, double label)
    {
        sumWeights += weight;
        sumWeightedLabels += weight * label;
    }

    template<typename LossFunctionType>
    ForestTrainer<LossFunctionType>::ExampleMetaData::ExampleMetaData(const dataset::WeightLabel & weightLabel) : dataset::WeightLabel(weightLabel), currentForestOutput(0)
    {} 

    template<typename LossFunctionType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeForestTrainer(const LossFunctionType& lossFunction, const ForestTrainerParameters& parameters)
    {
        return std::make_unique<ForestTrainer<LossFunctionType>>(lossFunction, parameters);
    }

    //
    // debugging code
    //
    
    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::Sums::Print(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "sumWeights = " << sumWeights << ", sumWeightedLabels = " << sumWeightedLabels;
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::NodeStats::Print(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "stats:\n";
        sums.Print(os, tabs+1);
        os << "\n";
        sums0.Print(os, tabs+1);
        os << "\n";
        sums1.Print(os, tabs+1);
        os << "\n";
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::SplitCandidate::Print(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "gain = " << gain << "\n";
        splitAction.Print(os, tabs);
        nodeStats.Print(os, tabs);
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::PriorityQueue::Print(std::ostream& os) const
    {
        os << "Priority Queue Size: " << size() << "\n";

        for(const auto& candidate : std::priority_queue<SplitCandidate>::c) // c is a protected member of std::priority_queue
        {
            os << "\n";
            candidate.Print(os, 1);
            os << "\n";
        }
    }
}
