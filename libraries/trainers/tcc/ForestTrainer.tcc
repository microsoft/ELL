////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//#define VERBOSE_MODE( x ) x   // uncomment this for very verbose mode
#define VERBOSE_MODE( x )       // uncomment this for nonverbose mode

namespace trainers
{    
    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::ForestTrainer(const BoosterType& booster, const ForestTrainerParameters& parameters) :
        _booster(booster), _parameters(parameters), _forest(std::make_shared<predictors::SimpleForestPredictor>())
    {}

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Update(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        // convert data from iterator to dense dataset with metadata (weak weight / weak label) associated with each example
        LoadData(exampleIterator);

        // boosting loop (outer loop)
        for(size_t round = 0; round < _parameters.numRounds; ++round)
        {
            // call the booster and compute sums for the entire dataset
            Sums sums = SetWeakWeightsLabels();

            // use the computed sums to calaculate the bias term, set it in the forest and the dataset
            double bias = sums.GetMeanLabel();
            _forest->AddToBias(bias);
            UpdateCurrentOutputs(bias);

            VERBOSE_MODE(_dataset.Print(std::cout));
            VERBOSE_MODE(std::cout << "\nBoosting iteration\n");
            VERBOSE_MODE(_forest->PrintLine(std::cout, 1));

            // find split candidate for root node and push it onto the priority queue
            auto rootSplit = GetBestSplitCandidateAtNode(_forest->GetNewRootId(), Range{0, _dataset.NumExamples()}, sums);

            // check for positive gain 
            if(rootSplit.gain < _parameters.minSplitGain || _parameters.maxSplitsPerRound == 0)
            {
                return;
            }

            // reset the queue and add the root split from the graph
            if(_queue.size() > 0)
            {
                _queue = PriorityQueue();
            }
            _queue.push(std::move(rootSplit));

            // start performing splits until the maximum is reached or the queue is empty
            PerformSplits(_parameters.maxSplitsPerRound);
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeRanges::NodeRanges(const Range& totalRange) : _total(totalRange)
    {}

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Range ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeRanges::GetChildRange(size_t childPosition) const
    {
        if (childPosition == 0)
        {
            return Range{ _total.firstIndex, _size0 };
        }
        else
        {
            return Range{ _total.firstIndex+_size0, _total.size-_size0 };
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeRanges::SetSize0(size_t value)
    {
        _size0 = value;
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeStats::SetChildSums(std::vector<Sums> childSums) 
    { 
        _childSums = childSums; 
    } 

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeStats::NodeStats(const Sums& totalSums) : _totalSums(totalSums), _childSums(2)
    {}

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    typename const ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Sums& ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeStats::GetChildSums(size_t position) const
    {
        return _childSums[position];
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplitCandidate::SplitCandidate(SplittableNodeId nodeId, Range totalRange, Sums totalSums) : gain(0), nodeId(nodeId), ranges(totalRange), stats(totalSums)
    {}

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::LoadData(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        // reset the dataset
        _dataset = dataset::RowDataset<ForestTrainerExample>();

        // create the dataset from the example iterator
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();

            auto denseDataVector = std::make_unique<dataset::DoubleDataVector>(example.GetDataVector().ToArray());

            ExampleMetaData metaData;
            metaData.strong = example.GetMetaData();
            metaData.currentOutput = _forest->Predict(*denseDataVector);

            _dataset.AddExample(ForestTrainerExample(std::move(denseDataVector), metaData));

            exampleIterator.Next();
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Sums ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SetWeakWeightsLabels()
    {
        Sums sums;

        for (uint64_t rowIndex = 0; rowIndex < _dataset.NumExamples(); ++rowIndex)
        {
            auto& metaData = _dataset[rowIndex].GetMetaData();
            metaData.weak = _booster.GetWeakWeightLabel(metaData.strong, metaData.currentOutput);
            sums.Increment(metaData.weak);
        }

        if(sums.sumWeights == 0.0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badData, "sum of weights in data is zero");
        }

        return sums;
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::UpdateCurrentOutputs(double value)
    {
        for (uint64_t rowIndex = 0; rowIndex < _dataset.NumExamples(); ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetaData().currentOutput += value;
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::UpdateCurrentOutputs(Range range, const EdgePredictorType& edgePredictor)
    {
        for (uint64_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size; ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetaData().currentOutput += edgePredictor.Predict(example.GetDataVector());
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::PerformSplits(size_t maxSplits)
    {
        // count splits
        size_t splitCount = 0;

        // splitting loop (inner loop)
        while (!_queue.empty())
        {
            VERBOSE_MODE(std::cout << "\nSplit iteration\n");
            VERBOSE_MODE(_queue.PrintLine(std::cout, 1));

            auto splitCandidate = _queue.top();
            _queue.pop();

            const auto& stats = splitCandidate.stats;
            const auto& ranges = splitCandidate.ranges;

            // sort the data according to the performed split and update the metadata to reflect this change
            SortNodeDataset(ranges.GetTotalRange(), splitCandidate.splitRule);

            // update current output field in metadata
            auto edgePredictors = GetEdgePredictors(stats);
            UpdateCurrentOutputs(ranges.GetChildRange(0), edgePredictors[0]);
            UpdateCurrentOutputs(ranges.GetChildRange(1), edgePredictors[1]);

            // have the forest perform the split
            using SplitAction = predictors::SimpleForestPredictor::SplitAction;
            SplitAction splitAction(splitCandidate.nodeId, splitCandidate.splitRule, edgePredictors);
            auto interiorNodeIndex = _forest->Split(splitAction);

            VERBOSE_MODE(_dataset.Print(std::cout, 1));
            VERBOSE_MODE(std::cout << "\n");
            VERBOSE_MODE(_forest->PrintLine(std::cout, 1));

            // if max number of splits reached, exit the loop
            if (++splitCount >= maxSplits)
            {
                break;
            }

            // queue new split candidates
            for (size_t i = 0; i<2; ++i)
            {
                auto splitCandidate = GetBestSplitCandidateAtNode(_forest->GetChildId(interiorNodeIndex, i), ranges.GetChildRange(i), stats.GetChildSums(i));
                if (splitCandidate.gain > _parameters.minSplitGain)
                {
                    _queue.push(std::move(splitCandidate));
                }
            }
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SortNodeDataset(Range range, const SplitRuleType& splitRule)
    {
        if(splitRule.NumOutputs() == 2)
        {
            _dataset.Partition([splitRule](const ForestTrainerExample& example) { return splitRule.Predict(example.GetDataVector()) == 0 ? true : false; },
                               range.firstIndex,
                               range.size);
        }
        else
        {
            _dataset.Sort([splitRule](const ForestTrainerExample& example) { return splitRule.Predict(example.GetDataVector()); },
                          range.firstIndex,
                          range.size);
        }
    }

    //
    // debugging code
    //
 
    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::ExampleMetaData::Print(std::ostream & os) const
    {
        os << "(" << strong.weight << ", " << strong.label << ", " << weak.weight << ", " << weak.label << ", " << currentOutput << ")";
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeStats::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "stats:\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums:\t";
        _totalSums.Print(os);
        os << "\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums0:\t";
        _childSums[0].Print(os);
        os << "\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums1:\t";
        _childSums[1].Print(os);
        os << "\n";
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplitCandidate::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "gain = " << gain << "\n";
        os << std::string(tabs * 4, ' ') << "node = ";
        nodeId.Print(os);
        os << "\n";
        splitRule.PrintLine(os, tabs);
        stats.PrintLine(os, tabs);
    }

    template<typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::PriorityQueue::PrintLine(std::ostream& os, size_t tabs) const
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
