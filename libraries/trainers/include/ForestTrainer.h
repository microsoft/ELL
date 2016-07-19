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
    /// <summary> Parameters for the forest trainer. </summary>
    struct ForestTrainerParameters
    {
        double minSplitGain = 0.0;
    };

    class ForestTrainerBase
    {
    protected:

        // Represents a range in an array
        struct Range
        {
            size_t firstIndex;
            size_t size;
        };

        // A struct that describes the range of training examples associated with a given node and its children
        class NodeRanges // TODO: document
        {
        public:
            NodeRanges(const Range& totalRange);
            Range GetTotalRange() const { return _total; }
            Range GetChildRange(size_t childPosition) const;
            void SetSize0(size_t value);

        private:
            Range _total;
            size_t _size0;
        };

        // Metadata that the forest trainer keeps with each example
        struct ExampleMetaData : public dataset::WeightLabel
        {
            ExampleMetaData(const dataset::WeightLabel& weightLabel);

            // the output of the forest on this example
            double currentOutput = 0;
            double weakLabel = 0;
            double weakWeight = 1;
        };

        // struct used to keep histograms of tree nodes
        struct Sums
        {
            double sumWeights = 0;
            double sumWeightedLabels = 0;

            void Increment(const ExampleMetaData& metaData);
            Sums operator-(const Sums& other) const;
            void Print(std::ostream& os) const;
        };

        // struct used to keep statistics about tree nodes
        struct NodeStats
        {
            Sums sums; // TODO rename totalSums
            Sums sums0;
            Sums sums1;

            NodeStats(const Sums& totalSums) : sums(totalSums)
            {}

            const Sums& GetTotalSums() { return sums; }

            const Sums& GetChildSums(size_t position) const // TODO: fix this!!!!!
            {
                if(position ==0) return sums0;
                return sums1;
            }

            void PrintLine(std::ostream& os, size_t tabs=0) const;
        };
    };

    /// <summary>
    /// Implements a greedy forest growing algorithm.
    /// </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to optimize. </typeparam>
    template <typename LossFunctionType> 
    class ForestTrainer : public ForestTrainerBase, public IIncrementalTrainer<predictors::SimpleForestPredictor>
    {
    public:

        using SplitRuleType = predictors::SingleInputThresholdRule;
        using EdgePredictorType = predictors::ConstantPredictor;

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

        // struct used to keep info about the gain maximizing split of each splittable node in the tree
        struct SplitCandidate                                                   // TODO: this depends on template param SplitRuleType
        {
            SplitCandidate(SplittableNodeId nodeId, Range totalRange, Sums sums);
            bool operator<(const SplitCandidate& other) const { return gain < other.gain; }
            void PrintLine(std::ostream& os, size_t tabs = 0) const;

            double gain;
            SplittableNodeId nodeId;
            SplitRuleType splitRule;
            NodeStats stats;
            NodeRanges ranges;
        };

        // implements a priority queue of split candidates that can print itself (useful for debugging)
        struct PriorityQueue : public std::priority_queue<SplitCandidate>
        {
            void PrintLine(std::ostream& os, size_t tabs=0) const;
            using std::priority_queue<SplitCandidate>::size;
        };

        // the type of example used by the forest trainer
        typedef dataset::Example<dataset::DoubleDataVector, ExampleMetaData> ForestTrainerExample; 

        Sums LoadData(dataset::GenericRowDataset::Iterator exampleIterator);

        void AddToCurrentOutput(Range range, const EdgePredictorType& edgePredictor);

        SplitCandidate GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums);

        void SortNodeDataset(Range range, size_t featureIndex);
        void SortNodeDataset(Range range, const SplitRuleType& splitRule);

        std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats);

        double CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const;
        double GetOutputValue(const Sums& sums) const;

        // member variables
        LossFunctionType _lossFunction;
        ForestTrainerParameters _parameters;

        // the forest being trained
        std::shared_ptr<predictors::SimpleForestPredictor> _forest;

        // local copy of the dataset, with metadata attached to each example
        dataset::RowDataset<ForestTrainerExample> _dataset;

        // priority queue used to identify the gain-maximizing split candidate
        PriorityQueue _queue;
    };
}

#include "../tcc/ForestTrainer.tcc"
