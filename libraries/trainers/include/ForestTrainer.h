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

// utilities
#include "OutputStreamImpostor.h"

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
        size_t maxSplitsPerRound = 0;
        size_t numRounds = 0;
    };

    class ForestTrainerBase
    {
    protected:
        // keeps track of the total weight and total weight-weak-label in a set of examples
        struct Sums
        {
            double sumWeights = 0;
            double sumWeightedLabels = 0;

            void Increment(const dataset::WeightLabel& weightLabel);
            Sums operator-(const Sums& other) const;
            double GetMeanLabel() const;
            void Print(std::ostream& os) const;
        };

        // represents a range in an array
        struct Range
        {
            size_t firstIndex;
            size_t size;
        };
    };

    /// <summary>
    /// Implements a greedy forest growing algorithm.
    /// </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to optimize. </typeparam>
    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType> 
    class ForestTrainer : public ForestTrainerBase, public IIncrementalTrainer<predictors::ForestPredictor<SplitRuleType, EdgePredictorType>> 
    { 
    public:
        /// <summary> Constructs an instance of ForestTrainer. </summary>
        ///
        /// <param name="booster"> The booster. </param>
        /// <param name="parameters"> Training Parameters. </param>
        ForestTrainer(const BoosterType& booster, const ForestTrainerParameters& parameters);

        /// <summary> Grows the decision forest. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set.  </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>> GetPredictor() const override { return _forest; };

    protected:       
        //
        // Private internal structs 
        //


        // describes the range of training examples associated with a given node and its children
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

        // metadata that the forest trainer keeps with each example
        struct ExampleMetadata 
        {
            void Print(std::ostream& os) const;

            // strong weight and label
            dataset::WeightLabel strong;

            // weak weight and label
            dataset::WeightLabel weak;

            // the output of the forest on this example
            double currentOutput = 0;
        };

        // keeps statistics about tree nodes
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
        
        // node identifier - borrowed from the forest predictor class 
        using SplittableNodeId = predictors::SimpleForestPredictor::SplittableNodeId;

        // keeps info about the gain maximizing split of each splittable node in the forest - the greedy algo maintains a priority queue of these
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

        // a priority queue of SplitCandidates
        struct PriorityQueue : public std::priority_queue<SplitCandidate>
        {
            void PrintLine(std::ostream& os, size_t tabs=0) const;
            using std::priority_queue<SplitCandidate>::size;
        };

        //
        // private member functions
        // 

        // loads a dataset and initializes the currentOutput field in the metadata
        void LoadData(dataset::GenericRowDataset::Iterator exampleIterator);

        // performs an epoch of splits
        void PerformSplits(size_t maxSplits);

        // runs the booster and sets the weak weight and weak labels
        Sums SetWeakWeightsLabels();

        // updates the currentOutput field in the metadata of a range of examples
        void UpdateCurrentOutputs(double value);
        void UpdateCurrentOutputs(Range range, const EdgePredictorType& edgePredictor);

        // after performing a split, we rearrange the dataset to ensure that each node's examples occupy contiguous rows in the dataset
        void SortNodeDataset(Range range, const SplitRuleType& splitRule); // TODO implement bucket sort

        //
        // implementation specific functions that must be implemented by a derived class
        // 

        virtual SplitCandidate GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums) = 0;
        virtual std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats) = 0;
        
        //
        // member variables
        //

        // user defined parameters 
        BoosterType _booster;
        ForestTrainerParameters _parameters;

        // the forest being grown
        std::shared_ptr<predictors::ForestPredictor<SplitRuleType, EdgePredictorType>> _forest;

        // the priority queue that holds the split candidates
        PriorityQueue _queue;

        // the dataset
        using ForestTrainerExample = dataset::Example<dataset::DoubleDataVector, ExampleMetadata>; 
        dataset::RowDataset<ForestTrainerExample> _dataset;
    };
}

#include "../tcc/ForestTrainer.tcc"
