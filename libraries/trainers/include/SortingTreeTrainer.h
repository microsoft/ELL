////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingTreeTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IBlackBoxTrainer.h"

// dataset
#include "RowDataset.h"
#include "DenseDataVector.h"

// predictors
#include "DecisionTreePredictor.h"

// stl
#include <queue>
#include <iostream>

/// <summary> trainers namespace </summary>
namespace trainers
{
    /// <summary> Parameters for the sorting tree trainer. </summary>
    struct SortingTreeTrainerParameters
    {
        double minSplitGain = 0.0;
    };

    /// <summary>
    /// Implements a greedy decision tree growing algorithm that operates by repeatedly sorting the
    /// data by each feature.
    /// </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to optimize. </typeparam>
    template <typename LossFunctionType> 
    class SortingTreeTrainer : public IBlackBoxTrainer<predictors::DecisionTreePredictor>
    {
    public:

        /// <summary> Constructs an instance of SortingTreeTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="parameters"> Training Parameters. </param>
        SortingTreeTrainer(const LossFunctionType& lossFunction, const SortingTreeTrainerParameters& parameters);

        /// <summary> Trains a decision tree. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set.  </param>
        ///
        /// <returns> The trained decision tree. </returns>
        virtual predictors::DecisionTreePredictor Train(dataset::GenericRowDataset::Iterator exampleIterator) const override;

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
            predictors::DecisionTreePredictor::Node* leaf;
            predictors::DecisionTreePredictor::SplitRule splitRule;
            double gain = 0;
            uint64_t fromRowIndex;
            uint64_t size;
            uint64_t negativeSize;
            Sums sums;
            Sums negativeSums;

            bool operator<(const SplitCandidate& other) const { return gain > other.gain; }
            void Print(std::ostream& os, const dataset::RowDataset<dataset::DoubleDataVector>& dataset) const;
        };

        struct PriorityQueue : public std::priority_queue<SplitCandidate>
        {
            void Print(std::ostream& os, const dataset::RowDataset<dataset::DoubleDataVector>& dataset) const;
            using std::priority_queue<SplitCandidate>::size;
        };

        Sums LoadData(dataset::GenericRowDataset::Iterator exampleIterator) const;
        void AddSplitCandidateToQueue(predictors::DecisionTreePredictor::Node* leaf, uint64_t fromRowIndex, uint64_t size, Sums sums) const;
        void SortDatasetByFeature(uint64_t featureIndex, uint64_t fromRowIndex, uint64_t size) const;
        double CalculateGain(Sums sums, Sums negativeSums) const;
        double GetOutputValue(Sums sums) const;
        void Cleanup() const;

        // member variables
        LossFunctionType _lossFunction;
        SortingTreeTrainerParameters _parameters;
        mutable dataset::RowDataset<dataset::DoubleDataVector> _dataset;
        mutable PriorityQueue _queue;
    };

    /// <summary> Makes a sorting tree trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="parameters"> The trainer parameters. </param>
    /// <param name="lossFunction"> The loss function. </param>
    ///
    /// <returns> A nique_ptr to a sorting tree trainer. </returns>
    template<typename LossFunctionType>
    std::unique_ptr<IBlackBoxTrainer<predictors::DecisionTreePredictor>> MakeSortingTreeTrainer(const LossFunctionType& lossFunction, const SortingTreeTrainerParameters& parameters);
}

#include "../tcc/SortingTreeTrainer.tcc"
