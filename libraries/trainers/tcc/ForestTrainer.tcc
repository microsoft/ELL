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
        AddSplitCandidateToQueue(_forest->GetNewRootId(), Range{ 0, _dataset.NumExamples() }, sums);

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

            // compute the constant output values on the new edges
            double output = GetOutputValue(stats.sums); 
            double output0 = GetOutputValue(stats.sums0) - output;
            double output1 = GetOutputValue(stats.sums1) - output;

            // perform the split
            std::vector<EdgePredictorType> edgePredictorVector{ output0, output1 };
            SplitAction splitAction(splitCandidate.nodeId, splitCandidate.splitRule, std::move(edgePredictorVector));
            auto interiorNodeIndex = _forest->Split(splitAction);

            // sort the data according to the performed split and update the metadata to reflect this change
            SortNodeDataset(ranges.GetTotalRange(), splitCandidate.splitRule);
            AddToCurrentOutput(ranges.GetChildRange(0), output0);
            AddToCurrentOutput(ranges.GetChildRange(1), output1);

            // queue split candidate for child 0
            AddSplitCandidateToQueue(_forest->GetChildId(interiorNodeIndex, 0), ranges.GetChildRange(0), stats.sums0);

            // queue split candidate for child 1
            AddSplitCandidateToQueue(_forest->GetChildId(interiorNodeIndex, 1), ranges.GetChildRange(1), stats.sums1);
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
    void ForestTrainer<LossFunctionType>::AddToCurrentOutput(Range range, double addValue)
    {
        for (uint64_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size; ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetaData().currentOutput += addValue;
        }
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::AddSplitCandidateToQueue(SplittableNodeId nodeId, Range range, Sums sums)
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
            for (uint64_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size-1; ++rowIndex)
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
                    bestSplitCandidate.stats.sums0 = sums0;
                    bestSplitCandidate.stats.sums1 = sums1;
                }
            }
        }

        // if found a good split candidate, queue it
        if (bestSplitCandidate.gain > 0.0)
        {
            _queue.push(std::move(bestSplitCandidate));
        }

#ifdef VERY_VERBOSE
        else
        {
            std::cout << "No positive-gain split candidate found - queue unmodified\n";
        }
#endif
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
    void trainers::ForestTrainer<LossFunctionType>::Sums::Increment(const ExampleMetaData& metaData)
    {
        sumWeights += metaData.weakWeight;
        sumWeightedLabels += metaData.weakWeight * metaData.weakLabel;
    }

    template<typename LossFunctionType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeForestTrainer(const LossFunctionType& lossFunction, const ForestTrainerParameters& parameters)
    {
        return std::make_unique<ForestTrainer<LossFunctionType>>(lossFunction, parameters);
    }

    template<typename LossFunctionType>
    ForestTrainer<LossFunctionType>::SplitCandidate::SplitCandidate(SplittableNodeId nodeId, Range totalRange, Sums sums) : gain(0), nodeId(nodeId), ranges(totalRange)
    {
        stats.sums = sums;
    }
    
    //
    // debugging code
    //
    
    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::Sums::Print(std::ostream& os) const
    {
        os << "sumWeights = " << sumWeights << ", sumWeightedLabels = " << sumWeightedLabels;
    }

    template<typename LossFunctionType>
    void ForestTrainer<LossFunctionType>::NodeStats::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "stats:\n";
        
        os << std::string((tabs+1) * 4, ' ') <<  "sums:\t";
        sums.Print(os);
        os << "\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums0:\t";
        sums0.Print(os);
        os << "\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums1:\t";
        sums1.Print(os);
        os << "\n";
    }
 
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
