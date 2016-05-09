////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingTreeTrainer.h (trainers)
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
    /// <summary> Implements a greedy decision tree growing algorithm that operates by repeatedly sorting the data by each feature. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to optimize. </typeparam>
    template <typename LossFunctionType>
    class SortingTreeTrainer
    {
    public:

        /// <summary> Constructs an instance of SortingTreeTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        SortingTreeTrainer(LossFunctionType lossFunction);

        /// <summary> Trains a decision tree. </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of the example iterator. </typeparam>
        /// <param name="exampleIterator"> [in,out] The example iterator. </param>
        ///
        /// <returns> A decision tree. </returns>
        template <typename ExampleIteratorType>
        predictors::DecisionTree Train(ExampleIteratorType exampleIterator);

    private:
        // struct used to keep statistics about tree leaves
        struct Sums
        {
            double sumWeights = 0;
            double sumWeightedLabels = 0;

            Sums operator-(const Sums& other) const; 
        };

        // struct used to keep info about the gain maximizing split of each leaf in the tree
        struct SplitCandidate
        {
            predictors::DecisionTree::Node* leaf;
            predictors::DecisionTree::SplitRule splitRule;
            double gain = 0;
            uint64_t fromRowIndex;
            uint64_t size;
            uint64_t negativeSize;
            Sums sums;
            Sums negativeSums;

            bool operator<(const SplitCandidate& other) const { return gain > other.gain; }
        };

        template <typename ExampleIteratorType>
        Sums LoadData(ExampleIteratorType exampleIterator);
        void AddSplitCandidateToQueue(predictors::DecisionTree::Node* leaf, uint64_t fromRowIndex, uint64_t size, Sums sums);
        void SortDatasetByFeature(uint64_t featureIndex, uint64_t fromRowIndex, uint64_t size);
        double CalculateGain(Sums sums, Sums negativeSums) const;
        double GetOutputValue(Sums sums) const;
        void Cleanup();

        // member variables
        LossFunctionType _lossFunction;
        dataset::RowDataset<dataset::DoubleDataVector> _dataset;
        std::priority_queue<SplitCandidate> _queue;
    };
}

#include "../tcc/SortingTreeTrainer.tcc"
