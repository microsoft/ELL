////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIncrementalTrainer.h"

// dataset
#include "RowDataset.h"
#include "DenseDataVector.h"

// predictors
#include "ForestPredictor.h"

// stl
#include <queue>
#include <memory>
#include <iostream>

/// <summary> trainers namespace </summary>
namespace trainers
{
    /// <summary> Parameters for the sorting tree trainer. </summary>
    struct ForestTrainerParameters
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
    class ForestTrainer : public IIncrementalTrainer<predictors::SimpleForestPredictor>
    {
    public:

        /// <summary> Constructs an instance of ForestTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="parameters"> Training Parameters. </param>
        ForestTrainer(const LossFunctionType& lossFunction, const ForestTrainerParameters& parameters);

        /// <summary> Trains a decision tree. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set.  </param>
        ///
        /// <returns> The trained decision tree. </returns>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const predictors::SimpleForestPredictor> GetPredictor() const { return _forest; };

    private:
        // Specify how the trainer identifies which node it is splitting. 
        using SplittableNodeId = predictors::SimpleForestPredictor::SplittableNodeId;

        // Specify how the trainer defines a split.
        using SplitAction = predictors::SimpleForestPredictor::SplitAction;

        // struct used to keep histograms of tree nodes
        struct Sums
        {
            double sumWeights = 0;
            double sumWeightedLabels = 0;

            Sums operator-(const Sums& other) const; 
        };

        // struct used to keep statistics about tree nodes
        struct NodeStats
        {
            uint64_t fromRowIndex;
            uint64_t size;
            uint64_t size0;
            Sums sums;
            Sums sums0;
            Sums sums1;
        };

        // struct used to keep info about the gain maximizing split of each splittable node in the tree
        struct SplitCandidate
        {
            SplitAction splitAction;
            SplittableNodeId nodeId;
            NodeStats nodeStats;

            double gain;

            size_t featureIndex;
            double threshold;

            bool operator<(const SplitCandidate& other) const { return gain > other.gain; }
            void Print(std::ostream& os, const dataset::RowDataset<dataset::DoubleDataVector>& dataset) const;
        };

        struct PriorityQueue : public std::priority_queue<SplitCandidate>
        {
            void Print(std::ostream& os, const dataset::RowDataset<dataset::DoubleDataVector>& dataset) const;
            using std::priority_queue<SplitCandidate>::size;
        };

        Sums LoadData(dataset::GenericRowDataset::Iterator exampleIterator);
        void AddSplitCandidateToQueue(SplittableNodeId nodeId, uint64_t fromRowIndex, uint64_t size, Sums sums);
        void SortDatasetByFeature(uint64_t featureIndex, uint64_t fromRowIndex, uint64_t size);
        double CalculateGain(Sums sums, Sums sums0) const;
        double GetOutputValue(Sums sums) const;

        // member variables
        LossFunctionType _lossFunction;
        ForestTrainerParameters _parameters;
        std::shared_ptr<predictors::SimpleForestPredictor> _forest;

        dataset::RowDataset<dataset::DoubleDataVector> _dataset;
        PriorityQueue _queue;
    };

    /// <summary> Makes a sorting tree trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="parameters"> The trainer parameters. </param>
    /// <param name="lossFunction"> The loss function. </param>
    ///
    /// <returns> A nique_ptr to a sorting tree trainer. </returns>
    template<typename LossFunctionType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeForestTrainer(const LossFunctionType& lossFunction, const ForestTrainerParameters& parameters);
}

#include "../tcc/ForestTrainer.tcc"
