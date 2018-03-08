////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"

// data
#include "Dataset.h"
#include "DenseDataVector.h"

// predictors
#include "ForestPredictor.h"

// utilities
#include "OutputStreamImpostor.h"

// stl
#include <iostream> // For std::cout in VERBOSE_MODE
#include <memory>
#include <queue>

namespace ell
{
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

    /// <summary> Nontemplated base class for forest trainers, provides some reusable internal classes. </summary>
    class ForestTrainerBase
    {
    protected:
        // keeps track of the total weight and total weight-weak-label in a set of examples
        struct Sums
        {
            double sumWeights = 0;
            double sumWeightedLabels = 0;

            void Increment(const data::WeightLabel& weightLabel);
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

        // describes the range of training examples associated with a given node and its children
        class NodeRanges
        {
        public:
            NodeRanges(const Range& totalRange);
            Range GetTotalRange() const;
            Range GetChildRange(size_t childPosition) const;
            void SplitChildRange(size_t childPosition, size_t size);

        private:
            std::vector<size_t> _firstIndex;
        };

        // metadata that the forest trainer keeps with each example
        struct TrainerMetadata
        {
            TrainerMetadata(const data::WeightLabel& metaData);
            void Print(std::ostream& os) const;

            // strong weight and label
            data::WeightLabel strong;

            // weak weight and label
            data::WeightLabel weak;

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
            void PrintLine(std::ostream& os, size_t tabs = 0) const;

        private:
            Sums _totalSums;
            std::vector<Sums> _childSums;
        };
    };

    /// <summary>
    /// Implements a greedy forest growing algorithm.
    /// </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to optimize. </typeparam>
    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    class ForestTrainer : public ForestTrainerBase, public ITrainer<predictors::ForestPredictor<SplitRuleType, EdgePredictorType>>
    {
    public:
        using PredictorType = typename predictors::ForestPredictor<SplitRuleType, EdgePredictorType>;
        using DataVectorType = typename PredictorType::DataVectorType;
        using TrainerExampleType = data::Example<DataVectorType, TrainerMetadata>;

        /// <summary> Constructs an instance of ForestTrainer. </summary>
        ///
        /// <param name="booster"> The booster. </param>
        /// <param name="parameters"> Training Parameters. </param>
        ForestTrainer(const BoosterType& booster, const ForestTrainerParameters& parameters);

        /// <summary> Sets the trainer's dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        void SetDataset(const data::AnyDataset& anyDataset) override;

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        void Update() override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A const reference to the current predictor. </returns>
        const PredictorType& GetPredictor() const override { return _forest; };

    protected:
        //
        // Private internal structs
        //

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
        struct SplitCandidatePriorityQueue : public std::priority_queue<SplitCandidate>
        {
            void PrintLine(std::ostream& os, size_t tabs = 0) const;
            using std::priority_queue<SplitCandidate>::size;
        };

        //
        // private member functions
        //

        // performs an epoch of splits
        void PerformSplits(size_t maxSplits);

        // runs the booster and sets the weak weight and weak labels
        Sums SetWeakWeightsLabels();

        // updates the currentOutput field in the metadata of a range of examples
        void UpdateCurrentOutputs(double value);
        void UpdateCurrentOutputs(Range range, const EdgePredictorType& edgePredictor);

        // after performing a split, we rearrange the data set to ensure that each node's examples occupy contiguous rows in the dataset
        void SortNodeDataset(Range range, const SplitRuleType& splitRule);

        //
        // implementation specific functions that must be implemented by a derived class
        //

        virtual SplitCandidate GetBestSplitRuleAtNode(SplittableNodeId nodeId, Range range, Sums sums) = 0;
        virtual std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats) = 0;

        //
        // member variables
        //

        // user defined parameters
        BoosterType _booster;
        ForestTrainerParameters _parameters;

        // the forest being grown
        PredictorType _forest;

        // the priority queue that holds the split candidates
        SplitCandidatePriorityQueue _queue;

        // the data set
        data::Dataset<TrainerExampleType> _dataset;
    };
}
}

#include "../tcc/ForestTrainer.tcc"
