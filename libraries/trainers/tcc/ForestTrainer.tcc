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
        // convert data from iterator to dense dataset with metadata (weak weight / weak label) associated with each example
        LoadData(exampleIterator);

        Sums sums = SetWeakWeightsLabels();

        // TODO: LoadData, initialize "current output" along the way
        // --> this is the beginning of the bootinf loop
        // Boosting - set weak label and weak weight - compute sums here
        // Get bias: set it in forest and updae the current output

        // computes the bias term, sets it in the forest and the dataset
        double bias = sums.sumWeightedLabels / sums.sumWeights; // TODO: check for zero denominator
        _forest->AddToBias(bias);
        UpdateCurrentOutputs(bias);

#ifdef VERY_VERBOSE
        _dataset.Print(std::cout);
        std::cout << "\n";
        _forest->PrintLine(std::cout);
#endif

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
             std::cout << "\n==> Iteration\n";
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
                UpdateCurrentOutputs(ranges.GetChildRange(i), edgePredictors[i]);
            }

            // have the forest perform the split
            using SplitAction = predictors::SimpleForestPredictor::SplitAction;
            SplitAction splitAction(splitCandidate.nodeId, splitCandidate.splitRule, edgePredictors);
            auto interiorNodeIndex = _forest->Split(splitAction);

#ifdef VERY_VERBOSE
            _dataset.Print(std::cout);
            std::cout << "\n";
            _forest->PrintLine(std::cout);
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
    ForestTrainer<SplitRuleType, EdgePredictorType>::NodeRanges::NodeRanges(const Range& totalRange) : _total(totalRange)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    typename ForestTrainer<SplitRuleType, EdgePredictorType>::Range ForestTrainer<SplitRuleType, EdgePredictorType>::NodeRanges::GetChildRange(size_t childPosition) const
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

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::NodeRanges::SetSize0(size_t value)
    {
        _size0 = value;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestTrainer<SplitRuleType, EdgePredictorType>::ExampleMetaData::ExampleMetaData(const dataset::WeightLabel& weightLabel) : dataset::WeightLabel(weightLabel)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::Sums::Increment(const ExampleMetaData& metaData)
    {
        sumWeights += metaData.weakWeight;
        sumWeightedLabels += metaData.weakWeight * metaData.weakLabel;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    typename ForestTrainer<SplitRuleType, EdgePredictorType>::Sums ForestTrainer<SplitRuleType, EdgePredictorType>::Sums::operator-(const Sums& other) const
    {
        Sums difference;
        difference.sumWeights = sumWeights - other.sumWeights;
        difference.sumWeightedLabels = sumWeightedLabels - other.sumWeightedLabels;
        return difference;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::NodeStats::SetChildSums(std::vector<Sums> childSums) 
    { 
        _childSums = childSums; 
    } 

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestTrainer<SplitRuleType, EdgePredictorType>::NodeStats::NodeStats(const Sums& totalSums) : _totalSums(totalSums), _childSums(2)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    typename const ForestTrainer<SplitRuleType, EdgePredictorType>::Sums& ForestTrainer<SplitRuleType, EdgePredictorType>::NodeStats::GetChildSums(size_t position) const
    {
        return _childSums[position];
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestTrainer<SplitRuleType, EdgePredictorType>::SplitCandidate::SplitCandidate(SplittableNodeId nodeId, Range totalRange, Sums totalSums) : gain(0), nodeId(nodeId), ranges(totalRange), stats(totalSums)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::LoadData(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();


            // TODO this code breaks encapsulation - give ForestTrainer a ctor that takes an IDataVector
            auto denseDataVector = std::make_unique<dataset::DoubleDataVector>(example.GetDataVector().ToArray());

            ExampleMetaData metaData = example.GetMetaData();
            metaData.currentOutput = _forest->Compute(*denseDataVector);

            _dataset.AddExample(ForestTrainerExample(std::move(denseDataVector), metaData));

            exampleIterator.Next();
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    typename ForestTrainer<SplitRuleType, EdgePredictorType>::Sums ForestTrainer<SplitRuleType, EdgePredictorType>::SetWeakWeightsLabels()
    {
        Sums sums;

        for (uint64_t rowIndex = 0; rowIndex < _dataset.NumExamples(); ++rowIndex)
        {
            auto& metaData = _dataset[rowIndex].GetMetaData();
            metaData.weakLabel = metaData.label;
            metaData.weakWeight = metaData.weight;
            sums.Increment(metaData);
        }

        if(sums.sumWeights == 0.0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badData, "sum of weights in data is zero");
        }

        return sums;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::UpdateCurrentOutputs(double value)
    {
        for (uint64_t rowIndex = 0; rowIndex < _dataset.NumExamples(); ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetaData().currentOutput += value;
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::UpdateCurrentOutputs(Range range, const EdgePredictorType& edgePredictor)
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

    //
    // debugging code
    //
 
    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::ExampleMetaData::Print(std::ostream & os) const
    {
        os << "(" << weight << ", " << label << ", " << currentOutput << ", " << weakWeight << ", " << weakLabel << ")";
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::Sums::Print(std::ostream& os) const
    {
        os << "sumWeights = " << sumWeights << ", sumWeightedLabels = " << sumWeightedLabels;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestTrainer<SplitRuleType, EdgePredictorType>::NodeStats::PrintLine(std::ostream& os, size_t tabs) const
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
