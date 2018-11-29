////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"

#include <data/include/Dataset.h>
#include <data/include/DenseDataVector.h>

#include <predictors/include/ForestPredictor.h>

#include <utilities/include/OutputStreamImpostor.h>

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
    class ForestTrainer : public ForestTrainerBase
        , public ITrainer<predictors::ForestPredictor<SplitRuleType, EdgePredictorType>>
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
} // namespace trainers
} // namespace ell

#pragma region implementation

//#define VERBOSE_MODE( x ) x   // uncomment this for very verbose mode
#define VERBOSE_MODE(x) // uncomment this for nonverbose mode

namespace ell
{
namespace trainers
{
    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::ForestTrainer(const BoosterType& booster, const ForestTrainerParameters& parameters) :
        _booster(booster),
        _parameters(parameters),
        _forest()
    {
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SetDataset(const data::AnyDataset& anyDataset)
    {
        // materialize a dataset of dense DataVectors with metadata that contains both strong and weak weight and lables for each example
        _dataset = data::Dataset<TrainerExampleType>(anyDataset);

        // initalizes the special fields in the dataset metadata: weak weight and label, currentOutput
        for (size_t rowIndex = 0; rowIndex < _dataset.NumExamples(); ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            auto prediction = _forest.Predict(example.GetDataVector());
            auto& metadata = example.GetMetadata();
            metadata.currentOutput = prediction;
            metadata.weak = _booster.GetWeakWeightLabel(metadata.strong, prediction);
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Update()
    {
        // boosting loop (outer loop)
        for (size_t round = 0; round < _parameters.numRounds; ++round)
        {
            // call the booster and compute sums for the entire data set
            Sums sums = SetWeakWeightsLabels();

            // use the computed sums to calaculate the bias term, set it in the forest and the data set
            double bias = sums.GetMeanLabel();
            _forest.AddToBias(bias);
            UpdateCurrentOutputs(bias);

            VERBOSE_MODE(_dataset.Print(std::cout));
            VERBOSE_MODE(std::cout << "\nBoosting iteration\n");
            VERBOSE_MODE(_forest.PrintLine(std::cout, 1));

            // find split candidate for root node and push it onto the priority queue
            auto rootSplit = GetBestSplitRuleAtNode(_forest.GetNewRootId(), Range{ 0, _dataset.NumExamples() }, sums);

            // check for positive gain
            if (rootSplit.gain < _parameters.minSplitGain || _parameters.maxSplitsPerRound == 0)
            {
                return;
            }

            // reset the queue and add the root split from the graph
            if (_queue.size() > 0)
            {
                _queue = SplitCandidatePriorityQueue();
            }
            _queue.push(std::move(rootSplit));

            // start performing splits until the maximum is reached or the queue is empty
            PerformSplits(_parameters.maxSplitsPerRound);
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplitCandidate::SplitCandidate(SplittableNodeId nodeId, Range totalRange, Sums totalSums) :
        gain(0),
        nodeId(nodeId),
        stats(totalSums),
        ranges(totalRange)
    {
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    auto ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SetWeakWeightsLabels() -> Sums
    {
        Sums sums;

        for (size_t rowIndex = 0; rowIndex < _dataset.NumExamples(); ++rowIndex)
        {
            auto& metadata = _dataset[rowIndex].GetMetadata();
            metadata.weak = _booster.GetWeakWeightLabel(metadata.strong, metadata.currentOutput);
            sums.Increment(metadata.weak);
        }

        if (sums.sumWeights == 0.0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badData, "sum of weights in data is zero");
        }

        return sums;
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::UpdateCurrentOutputs(double value)
    {
        for (size_t rowIndex = 0; rowIndex < _dataset.NumExamples(); ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetadata().currentOutput += value;
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::UpdateCurrentOutputs(Range range, const EdgePredictorType& edgePredictor)
    {
        for (size_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size; ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetadata().currentOutput += edgePredictor.Predict(example.GetDataVector());
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::PerformSplits(size_t maxSplits)
    {
        // count splits
        size_t splitCount = 0;

        // splitting loop (inner loop)
        while (!_queue.empty())
        {
            VERBOSE_MODE(std::cout << "\nSplit iteration\n");
            VERBOSE_MODE(_queue.PrintLine(std::cout, 1));

            auto splitCandidate = _queue.top();
            _queue.pop();

            const auto& stats = splitCandidate.stats;
            const auto& ranges = splitCandidate.ranges;

            // sort the data according to the performed split and update the metadata to reflect this change
            SortNodeDataset(ranges.GetTotalRange(), splitCandidate.splitRule);

            // update current output field in metadata
            auto edgePredictors = GetEdgePredictors(stats);
            for (size_t i = 0; i < splitCandidate.splitRule.NumOutputs(); ++i)
            {
                UpdateCurrentOutputs(ranges.GetChildRange(i), edgePredictors[i]);
            }

            // have the forest perform the split
            using SplitAction = predictors::SimpleForestPredictor::SplitAction;
            SplitAction splitAction(splitCandidate.nodeId, splitCandidate.splitRule, edgePredictors);
            auto interiorNodeIndex = _forest.Split(splitAction);

            VERBOSE_MODE(_dataset.Print(std::cout, 1));
            VERBOSE_MODE(std::cout << "\n");
            VERBOSE_MODE(_forest.PrintLine(std::cout, 1));

            // if max number of splits reached, exit the loop
            if (++splitCount >= maxSplits)
            {
                break;
            }

            // queue new split candidates
            for (size_t i = 0; i < splitCandidate.splitRule.NumOutputs(); ++i)
            {
                auto splitCandidate = GetBestSplitRuleAtNode(_forest.GetChildId(interiorNodeIndex, i), ranges.GetChildRange(i), stats.GetChildSums(i));
                if (splitCandidate.gain > _parameters.minSplitGain)
                {
                    _queue.push(std::move(splitCandidate));
                }
            }
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SortNodeDataset(Range range, const SplitRuleType& splitRule)
    {
        if (splitRule.NumOutputs() == 2)
        {
            _dataset.Partition([splitRule](const data::Example<DataVectorType, TrainerMetadata>& example) { return splitRule.Predict(example.GetDataVector()) == 0; },
                               range.firstIndex,
                               range.size);
        }
        else
        {
            _dataset.Sort([splitRule](const data::Example<DataVectorType, TrainerMetadata>& example) { return splitRule.Predict(example.GetDataVector()); },
                          range.firstIndex,
                          range.size);
        }
    }

    //
    // debugging code
    //

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplitCandidatePriorityQueue::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "Priority Queue Size: " << size() << "\n";

        for (const auto& candidate : std::priority_queue<SplitCandidate>::c) // c is a protected member of std::priority_queue
        {
            os << "\n";
            candidate.PrintLine(os, tabs + 1);
            os << "\n";
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType, typename BoosterType>
    void ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplitCandidate::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "gain = " << gain << "\n";
        os << std::string(tabs * 4, ' ') << "node = ";
        nodeId.Print(os);
        os << "\n";
        splitRule.PrintLine(os, tabs);
        stats.PrintLine(os, tabs);
    }
} // namespace trainers
} // namespace ell

#pragma endregion implementation
