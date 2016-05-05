////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingTreeLearner.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"
#include "DenseDataVector.h"

// predictors
#include "DecisionTree.h"

// stl
#include <queue>

namespace trainers
{
    template <typename LossFunctionType>
    class SortingTreeLearner
    {
    public:

        /// <summary> Constructs an instance of SortingTreeLearner. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        SortingTreeLearner(LossFunctionType lossFunction);

        /// <summary> Trains a decision tree. </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of the example iterator. </typeparam>
        /// <param name="exampleIterator"> [in,out] The example iterator. </param>
        ///
        /// <returns> A decision tree. </returns>
        template <typename ExampleIteratorType>
        predictors::DecisionTree Train(ExampleIteratorType exampleIterator);

    private:

        struct Sums
        {
            double sumWeights = 0;
            double sumWeightedLabels = 0;

            Sums operator-(const Sums& other) const; 
        };

        struct SplitCandidate
        {
            predictors::DecisionTree::Child* leaf;
            predictors::DecisionTree::SplitRule splitRule;
            double gain = 0;
            uint64_t fromRowIndex;
            uint64_t negativeSize;
            uint64_t positiveSize;
            Sums negativeSums;
            Sums positiveSums;

            bool operator<(const SplitCandidate& other) const { return gain > other.gain; }
        };

        template <typename ExampleIteratorType>
        Sums LoadData(ExampleIteratorType exampleIterator);

        void AddSplitCandidateToQueue(predictors::DecisionTree::Child* leaf, uint64_t fromRowIndex, uint64_t size, Sums sums);

        double CalculateGain(Sums negativeSums, Sums positiveSums) const;

        void Cleanup();

        LossFunctionType _lossFunction;
        dataset::RowDataset<dataset::DoubleDataVector> _dataset;
        std::priority_queue<SplitCandidate> _queue;
    };
}

#include "../tcc/SortingTreeLearner.tcc"
