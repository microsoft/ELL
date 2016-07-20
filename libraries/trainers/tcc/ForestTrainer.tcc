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
    template <typename SplitRuleType, typename EdgePredictorType>
    ForestTrainer<SplitRuleType, EdgePredictorType>::ForestTrainer(const ForestTrainerParameters& parameters) :
        _parameters(parameters), _forest(std::make_shared<predictors::SimpleForestPredictor>())
    {}

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::Update(dataset::GenericRowDataset::Iterator exampleIterator) 
    {
        // convert data fron iterator to dense row dataset; compute sums statistics of the tree root
        auto sums = LoadData(exampleIterator);

        // computes the bias term, sets it in the forest and the dataset
        double bias = sums.sumWeightedLabels / sums.sumWeights; // TODO: check for zero denominator
        _forest->SetBias(bias);
        InitializeCurrentOutputs(bias);

        // find split candidate for root node and push it onto the priority queue
        auto rootSplit = GetBestSplitCandidateAtNode(_forest->GetNewRootId(), Range{ 0, _dataset.NumExamples() }, sums);

        // check for positive gain 
        if(rootSplit.gain < _parameters.minSplitGain || _parameters.maxSplitsPerEpoch == 0)
        {
            return; 
        }

        // count splits
        size_t splitCount = 0;

        // reset the queue and add the root split from the graph
        if(_queue.size() > 0)
        {
            _queue = PriorityQueue();
        }
        _queue.push(std::move(rootSplit));

        // as long as positive gains can be attained, keep growing the tree
        while(!_queue.empty()) // TODO: let user specify how many steps
        {

#ifdef VERY_VERBOSE
             std::cout << "Iteration\n";
             _queue.PrintLine(std::cout);
#endif

            auto splitCandidate = _queue.top();
            _queue.pop();

            const auto& stats = splitCandidate.stats;
            const auto& ranges = splitCandidate.ranges;

            // sort the data according to the performed split and update the metadata to reflect this change
            SortNodeDataset(ranges.GetTotalRange(), splitCandidate.splitRule);

            // update current output field in metadata
            auto edgePredictors = GetEdgePredictors(stats);
            for(size_t i = 0; i<2; ++i)
            {
                UpdateCurrentOutput(ranges.GetChildRange(i), edgePredictors[i]);
            }

            // have the forest perform the split
            using SplitAction = predictors::SimpleForestPredictor::SplitAction;
            SplitAction splitAction(splitCandidate.nodeId, splitCandidate.splitRule, edgePredictors);
            auto interiorNodeIndex = _forest->Split(splitAction);

#ifdef VERY_VERBOSE
            _dataset.Print(std::cout);
            // _forest->Print(std::cout);
#endif

            // if max number of splits reached, exit the loop
            if(++splitCount == _parameters.maxSplitsPerEpoch)
            {
                break;
            }

            // queue new split candidates
            for(size_t i = 0; i<2; ++i)
            {
                auto splitCandidate = GetBestSplitCandidateAtNode(_forest->GetChildId(interiorNodeIndex, i), ranges.GetChildRange(i), stats.GetChildSums(i));
                if(splitCandidate.gain > _parameters.minSplitGain)
                {
                    _queue.push(std::move(splitCandidate));
                }
            }
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::InitializeCurrentOutputs(double bias)
    {
        for(uint64_t rowIndex = 0; rowIndex < _dataset.NumExamples(); ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            auto& metaData = example.GetMetaData();
            metaData.currentOutput = _forest->Compute(example.GetDataVector()) + bias;
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::UpdateCurrentOutput(Range range, const EdgePredictorType& edgePredictor)
    {
        for (uint64_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size; ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetaData().currentOutput += edgePredictor.Compute(example.GetDataVector());
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::SortNodeDataset(Range range, const SplitRuleType& splitRule)
    {
        if(splitRule.NumOutputs() == 2)
        {
            _dataset.Partition([splitRule](const ForestTrainerExample& example) { return splitRule.Compute(example.GetDataVector()) == 0 ? true : false; },
                               range.firstIndex,
                               range.size);
        }
        else
        {
            _dataset.Sort([splitRule](const ForestTrainerExample& example) { return splitRule.Compute(example.GetDataVector()); },
                          range.firstIndex,
                          range.size);
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestTrainer<SplitRuleType, EdgePredictorType>::SplitCandidate::SplitCandidate(SplittableNodeId nodeId, Range totalRange, Sums totalSums) : gain(0), nodeId(nodeId), ranges(totalRange), stats(totalSums)
    {}
    
    //
    // debugging code
    //
 
    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::SplitCandidate::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "gain = " << gain << "\n";
        os << std::string(tabs * 4, ' ') << "node = ";
        nodeId.Print(os);
        os << "\n";
        splitRule.PrintLine(os, tabs);
        stats.PrintLine(os, tabs);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::PriorityQueue::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "Priority Queue Size: " << size() << "\n";

        for(const auto& candidate : std::priority_queue<SplitCandidate>::c) // c is a protected member of std::priority_queue
        {
            os << "\n";
            candidate.PrintLine(os, tabs + 1);
            os << "\n";
        }
    }
}
