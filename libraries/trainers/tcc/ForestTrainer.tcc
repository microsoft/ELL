#include "..\include\ForestTrainer.h"
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

        // computes the bias term, sets it in the forest and the dataset
        double bias = GetOutputValue(sums);
        _forest->SetBias(bias);
        AddToCurrentOutput(Range{ 0, _dataset.NumExamples() }, bias);

        // find split candidate for root node and push it onto the priority queue
        auto rootSplit = GetBestSplitCandidateAtNode(_forest->GetNewRootId(), Range{ 0, _dataset.NumExamples() }, sums);

        // check for positive gain 
        if(rootSplit.gain == 0)
        {
            return; // TODO - perhaps throw an exception
        }

        // add the root split from the graph
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

            auto edgePredictors = GetEdgePredictors(stats);

            using SplitAction = predictors::SimpleForestPredictor::SplitAction; // TODO: this depends on predictors and on split rules
            SplitAction splitAction(splitCandidate.nodeId, splitCandidate.splitRule, edgePredictors);
            auto interiorNodeIndex = _forest->Split(splitAction);

            for(size_t i = 0; i<2; ++i)
            {
                // update metadata in dataset
                AddToCurrentOutput(ranges.GetChildRange(i), edgePredictors[i]);

                // queue new split candidate
                auto splitCandidate = GetBestSplitCandidateAtNode(_forest->GetChildId(interiorNodeIndex, i), ranges.GetChildRange(i), stats.GetChildSums(i));
                if(splitCandidate.gain > 0.0)
                {
                    _queue.push(std::move(splitCandidate));
                }
            }
        }
    }

    template<typename LossFunctionType>
    typename ForestTrainer<LossFunctionType>::Sums ForestTrainer<LossFunctionType>::LoadData(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        Sums sums;

        // create DenseRowDataset
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();
            
            ExampleMetaData metaData = example.GetMetaData();
            
            // set weak label/weight to equal strong label/weight
            metaData.weakLabel = metaData.GetLabel();
            metaData.weakWeight = metaData.GetWeight();
            sums.Increment(metaData);

            // TODO this code breaks encapsulation - give ForestTrainer a ctor that takes an IDataVector
            auto denseDataVector = std::make_unique<dataset::DoubleDataVector>(example.GetDataVector().ToArray());
            auto forestTrainerExample = ForestTrainerExample(std::move(denseDataVector), metaData);
            _dataset.AddExample(std::move(forestTrainerExample));

            exampleIterator.Next();
        }

        return sums;
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::AddToCurrentOutput(Range range, const EdgePredictorType& edgePredictor)
    {
        for (uint64_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size; ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetaData().currentOutput += edgePredictor.Compute(example.GetDataVector());
        }
    }

    template<typename LossFunctionType>
    typename ForestTrainer<LossFunctionType>::SplitCandidate ForestTrainer<LossFunctionType>::GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums)
    {
        auto numFeatures = _dataset.GetMaxDataVectorSize();

        SplitCandidate bestSplitCandidate(nodeId, range, sums);

        for(uint64_t inputIndex = 0; inputIndex < numFeatures; ++inputIndex)
        {
            // sort the relevant rows of dataset in ascending order by inputIndex
            SortNodeDataset(range, inputIndex);

            Sums sums0;

            // consider all thresholds
            double nextFeatureValue = _dataset[range.firstIndex].GetDataVector()[inputIndex];
            for(uint64_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size-1; ++rowIndex)
            {
                // get friendly names
                double currentFeatureValue = nextFeatureValue;
                nextFeatureValue = _dataset[rowIndex + 1].GetDataVector()[inputIndex];

                // increment sums 
                sums0.Increment(_dataset[rowIndex].GetMetaData());

                // only split between rows with different feature values
                if(currentFeatureValue == nextFeatureValue)
                {
                    continue;
                }

                // compute sums1 and gain
                auto sums1 = sums - sums0;
                double gain = CalculateGain(sums, sums0, sums1);

                // find gain maximizer
                if(gain > bestSplitCandidate.gain)
                {
                    bestSplitCandidate.gain = gain;
                    bestSplitCandidate.splitRule = SplitRuleType{inputIndex, 0.5 * (currentFeatureValue + nextFeatureValue)};
                    bestSplitCandidate.ranges.SetSize0(rowIndex - range.firstIndex + 1);
                    bestSplitCandidate.stats.sums0 = sums0;
                    bestSplitCandidate.stats.sums1 = sums1;
                }
            }
        }

        return bestSplitCandidate;
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::SortNodeDataset(Range range, size_t inputIndex) // to be deprecated
    {
        _dataset.Sort([inputIndex](const ForestTrainerExample& example) { return example.GetDataVector()[inputIndex]; },
                      range.firstIndex,
                      range.size);
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::SortNodeDataset(Range range, const SplitRuleType& splitRule)
    {
        _dataset.Sort([splitRule](const ForestTrainerExample& example) { return splitRule.Compute(example.GetDataVector()); },
                      range.firstIndex,
                      range.size);
    }

    template<typename LossFunctionType>
    std::vector<typename ForestTrainer<LossFunctionType>::EdgePredictorType> ForestTrainer<LossFunctionType>::GetEdgePredictors(const NodeStats& nodeStats)
    {
        double output = GetOutputValue(nodeStats.sums);               
        double output0 = GetOutputValue(nodeStats.sums0) - output;    
        double output1 = GetOutputValue(nodeStats.sums1) - output;    
        return std::vector<EdgePredictorType>{ output0, output1 };
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
    ForestTrainer<LossFunctionType>::SplitCandidate::SplitCandidate(SplittableNodeId nodeId, Range totalRange, Sums totalSums) : gain(0), nodeId(nodeId), ranges(totalRange), stats(totalSums)
    {}
    
    //
    // debugging code
    //
 
    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::SplitCandidate::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "gain = " << gain << "\n";
        os << std::string(tabs * 4, ' ') << "node = ";
        nodeId.Print(os);
        os << "\n";
        splitRule.PrintLine(os, tabs);
        stats.PrintLine(os, tabs);
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::PriorityQueue::PrintLine(std::ostream& os, size_t tabs) const
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
