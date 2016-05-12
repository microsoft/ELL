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
#include "DecisionTreePredictor.h"

// stl
#include <queue>

/// <summary> trainers namespace </summary>
namespace trainers
{
    /// <summary> Interface to sorting tree trainer. </summary>
    class ISortingTreeTrainer
    {
    public:
        using ExampleIteratorType = dataset::GenericRowDataset::Iterator;

        virtual ~ISortingTreeTrainer() = default;

        /// <summary> Parameters for the sorting tree trainer. </summary>
        struct Parameters
        {
            double minSplitGain = 0.0;
        };

        /// <summary> Trains a decision tree. </summary>
        ///
        /// <param name="exampleIterator"> [in,out] The example iterator. </param>
        ///
        /// <returns> A decision tree. </returns>
        virtual predictors::DecisionTreePredictor Train(ExampleIteratorType exampleIterator) const = 0;
    };

    /// <summary> Implements a greedy decision tree growing algorithm that operates by repeatedly sorting the data by each feature. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to optimize. </typeparam>
    template <typename LossFunctionType> 
    class SortingTreeTrainer : public ISortingTreeTrainer
    {
    public:

        /// <summary> Constructs an instance of SortingTreeTrainer. </summary>
        ///
        /// <param name="parameters"> Training Parameters. </param>
        /// <param name="lossFunction"> The loss function. </param>
        SortingTreeTrainer(const ISortingTreeTrainer::Parameters& parameters, const LossFunctionType& lossFunction);

        /// <summary> Trains a decision tree. </summary>
        ///
        /// <param name="exampleIterator"> [in,out] The example iterator. </param>
        ///
        /// <returns> A decision tree. </returns>
        virtual predictors::DecisionTreePredictor Train(ISortingTreeTrainer::ExampleIteratorType exampleIterator) const override;

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
        };

        Sums LoadData(ExampleIteratorType exampleIterator) const;
        void AddSplitCandidateToQueue(predictors::DecisionTreePredictor::Node* leaf, uint64_t fromRowIndex, uint64_t size, Sums sums) const;
        void SortDatasetByFeature(uint64_t featureIndex, uint64_t fromRowIndex, uint64_t size) const;
        double CalculateGain(Sums sums, Sums negativeSums) const;
        double GetOutputValue(Sums sums) const;
        void Cleanup() const;

        // member variables
        ISortingTreeTrainer::Parameters _parameters;
        LossFunctionType _lossFunction;
        mutable dataset::RowDataset<dataset::DoubleDataVector> _dataset;
        mutable std::priority_queue<SplitCandidate> _queue;
    };

    /// <summary> Makes a sorting tree trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="parameters"> The trainer parameters. </param>
    /// <param name="lossFunction"> The loss function. </param>
    ///
    /// <returns> A sorting tree trainer. </returns>
    template<typename LossFunctionType>
    SortingTreeTrainer<LossFunctionType> MakeSortingTreeTrainer(const ISortingTreeTrainer::Parameters& parameters, const LossFunctionType& lossFunction);
}

#include "../tcc/SortingTreeTrainer.tcc"
