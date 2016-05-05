#include "..\include\SortingTreeLearner.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingTreeLearner.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{    
    template <typename LossFunctionType>
    SortingTreeLearner<LossFunctionType>::SortingTreeLearner(LossFunctionType lossFunction) : _lossFunction(lossFunction)
    {}

    template <typename LossFunctionType>
    template <typename ExampleIteratorType>
    predictors::DecisionTree SortingTreeLearner<LossFunctionType>::Train(ExampleIteratorType exampleIterator)
    {

        auto sums = LoadData(exampleIterator);

        predictors::DecisionTree tree;

        // find split candidate for root node
        AddSplitCandidateToQueue(&tree.GetRoot(), 0, _dataset.NumExamples(), sums);

        // as long as positive gains can be attained, keep growing the tree
        while(!_queue.empty())
        {
            const auto& splitInfo = _queue.top();

            // perform the split
            auto& interiorNode = splitInfo.leaf->Split(splitInfo.splitRule, 0, 0); // TODO add function called get LeafOutputValues(Sums)

            // sort the data according to the performed split

            // queue split candidate for negative child

            // queue split candidate for positive child

            _queue.pop();
        }



//        std::cout << _dataset << std::endl;

        // TODO - replace the below with a real tree learning algo
        //auto& root = tree.GetRoot().Split(predictors::DecisionTree::SplitRule{0, 0.0 }, -1.0, 1.0);
        //root.GetNegativeChild().Split(predictors::DecisionTree::SplitRule{1, 1.0}, -2.0, 2.0);
        //root.GetPositiveChild().Split(predictors::DecisionTree::SplitRule{2, 2.0}, -4.0, 4.0);

        Cleanup();

        return tree;
    }

    template<typename LossFunctionType>
    typename SortingTreeLearner<LossFunctionType>::Sums SortingTreeLearner<LossFunctionType>::Sums::operator-(const typename SortingTreeLearner<LossFunctionType>::Sums& other) const
    {
        Sums difference;
        difference.sumWeights = sumWeights - other.sumWeights;
        difference.sumWeightedLabels = sumWeightedLabels - other.sumWeightedLabels;
        return difference;
    }

    template<typename LossFunctionType>
    template <typename ExampleIteratorType>
    typename SortingTreeLearner<LossFunctionType>::Sums SortingTreeLearner<LossFunctionType>::LoadData(ExampleIteratorType exampleIterator)
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
    void SortingTreeLearner<LossFunctionType>::AddSplitCandidateToQueue(predictors::DecisionTree::Child* leaf, uint64_t fromRowIndex, uint64_t size, Sums sums)
    {
        auto numFeatures = _dataset.GetMaxDataVectorSize();

        SplitCandidate splitCandidate;
        splitCandidate.leaf = leaf;
        splitCandidate.fromRowIndex = fromRowIndex;

        for (uint64_t featureIndex = 0; featureIndex < numFeatures; ++featureIndex)
        {
            // sort the relevant rows of dataset in ascending order by featureIndex
            _dataset.Sort([featureIndex](const dataset::SupervisedExample<dataset::DoubleDataVector>& example) {return example.GetDataVector()[featureIndex];},
                fromRowIndex,
                size);
            
            Sums negativeSums;

            for (uint64_t rowIndex = fromRowIndex; rowIndex < fromRowIndex + size-1; ++rowIndex)
            {
                // get friendly names
                const auto& currentRow = _dataset[rowIndex].GetDataVector();
                const auto& nextRow = _dataset[rowIndex+1].GetDataVector();
                double weight = _dataset[rowIndex].GetWeight();
                double label = _dataset[rowIndex].GetLabel();

                // increment sums
                negativeSums.sumWeights += weight;
                negativeSums.sumWeightedLabels += weight * label;

                // only split between rows with different feature values
                if (currentRow[featureIndex] == nextRow[featureIndex])
                {
                    continue;
                }

                // subtract to get positive sums
                Sums positiveSums = sums - negativeSums;

                // find gain maximizer
                double gain = CalculateGain(negativeSums, positiveSums);
                if (gain > splitCandidate.gain)
                {
                    splitCandidate.gain = gain;
                    splitCandidate.splitRule.featureIndex = featureIndex;
                    splitCandidate.splitRule.threshold = 0.5 * (currentRow[featureIndex] + nextRow[featureIndex]);
                    splitCandidate.negativeSize = rowIndex + 1;
                    splitCandidate.positiveSize = size - rowIndex - 1;
                    splitCandidate.negativeSums = negativeSums;
                    splitCandidate.positiveSums = positiveSums;
                }
            }
        }

        // if found a good split candidate, queue it
        if (splitCandidate.gain > 0.0)
        {
            _queue.push(std::move(splitCandidate));
        }
    }

    template<typename LossFunctionType>
    double SortingTreeLearner<LossFunctionType>::CalculateGain(Sums negativeSums, Sums positiveSums) const
    {
        return 0.0;
    }

    template<typename LossFunctionType>
    void SortingTreeLearner<LossFunctionType>::Cleanup()
    {
        _dataset.Reset();
        while (!_queue.empty())
        {
            _queue.pop();
        }
    }
}
