////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     SortingTreeLearner.tcc (trainers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// dataset
#include "DenseSupervisedExample.h"
#include "RowDataset.h"

namespace trainers
{    
    template <typename LossFunctionType>
    SortingTreeLearner<LossFunctionType>::SortingTreeLearner(LossFunctionType lossFunction) : _lossFunction(lossFunction)
    {}

    template <typename LossFunctionType>
    template <typename ExampleIteratorType>
    predictors::DecisionTree SortingTreeLearner<LossFunctionType>::Train(ExampleIteratorType& exampleIterator, uint64_t maxDataVectorSize)
    {
        predictors::DecisionTree tree;

        // create DenseRowDataset
        dataset::RowDataset<dataset::DenseSupervisedExample> denseDataset;
        while (exampleIterator.IsValid())
        {
            denseDataset.AddExample(dataset::DenseSupervisedExample(exampleIterator.Get(), maxDataVectorSize));
            exampleIterator.Next();
        }


        // TODO - replace the below with a real tree learning algo
        auto& root = tree.SplitRoot(predictors::DecisionTree::SplitRule(0, 0.0), -1.0, 1.0);
        root.GetNegativeChild().Split(predictors::DecisionTree::SplitRule(1, 1.0), -2.0, 2.0);
        root.GetPositiveChild().Split(predictors::DecisionTree::SplitRule(2, 2.0), -4.0, 4.0);

        return tree;
    }
}
