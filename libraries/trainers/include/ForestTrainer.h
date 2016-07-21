////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO - second call to Update should run data through the forest and update the "currentOutput"
// 
// TODO - bucket sort
// TODO - add a Template booster that sets weak labels and weights
// TODO - add parameters to set epoch sizes, how often to reboost

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
        size_t maxSplitsPerEpoch = 0;
    };

    /// <summary> Base class for all forest trainers. </summary>
    class ForestTrainerBase
    {
    protected:
        // protected ctor - makes sure that a ForestTrainerBase is never created directly
        ForestTrainerBase(){}

        // Represents a range in an array
        struct Range
        {
            size_t firstIndex;
            size_t size;
        };

        // A struct that describes the range of training examples associated with a given node and its children
        class NodeRanges 
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
            void Print(std::ostream& os) const;

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
            NodeStats(const Sums& totalSums);
            const Sums& GetTotalSums() const { return _totalSums; }
            void SetChildSums(std::vector<Sums> childSums);
            const Sums& GetChildSums(size_t position) const;
            void PrintLine(std::ostream& os, size_t tabs=0) const;

        private:
            Sums _totalSums; 
            std::vector<Sums> _childSums;
        };

        // the type of example used by the forest trainer
        typedef dataset::Example<dataset::DoubleDataVector, ExampleMetaData> ForestTrainerExample; 

        // loads a dataset and computes its sums
        Sums LoadData(dataset::GenericRowDataset::Iterator exampleIterator);

        // local copy of the dataset, with metadata attached to each example
        dataset::RowDataset<ForestTrainerExample> _dataset;
    };

    /// <summary>
    /// Implements a greedy forest growing algorithm.
    /// </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to optimize. </typeparam>
    template <typename SplitRuleType, typename EdgePredictorType> 
    class ForestTrainer : public ForestTrainerBase, public IIncrementalTrainer<predictors::ForestPredictor<SplitRuleType, EdgePredictorType>> 
    { 
    public:
        /// <summary> Constructs an instance of ForestTrainer. </summary>
        ///
        /// <param name="parameters"> Training Parameters. </param>
        ForestTrainer(const ForestTrainerParameters& parameters);

        /// <summary> Grows the decision forest. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set.  </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>> GetPredictor() const { return _forest; };

    protected:
        // Specify how the trainer identifies which node it is splitting. 
        using SplittableNodeId = predictors::SimpleForestPredictor::SplittableNodeId;

        // struct used to keep info about the gain maximizing split of each splittable node in the tree
        struct SplitCandidate
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

        void InitializeCurrentOutputs();
        void UpdateCurrentOutput(Range range, const EdgePredictorType& edgePredictor);
        void SortNodeDataset(Range range, const SplitRuleType& splitRule); // TODO implement bucket sort

        virtual SplitCandidate GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums) = 0;
        virtual std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats) = 0;
        
        //
        // member variables
        //
        ForestTrainerParameters _parameters;

        // the forest
        std::shared_ptr<predictors::ForestPredictor<SplitRuleType, EdgePredictorType>> _forest;

        // priority queue used to identify the gain-maximizing split candidate
        PriorityQueue _queue;
    };
}

#include "../tcc/ForestTrainer.tcc"
