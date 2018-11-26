////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SortingForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ForestTrainer.h"
#include "LogitBooster.h"

#include <predictors/include/ConstantPredictor.h>
#include <predictors/include/SingleElementThresholdPredictor.h>

namespace ell
{
namespace trainers
{
    /// <summary> Parameters for the sorting forest trainer. </summary>
    struct SortingForestTrainerParameters : public virtual ForestTrainerParameters
    {
    };

    /// <summary> A trainer for binary decision forests with threshold split rules and constant outputs
    /// that operates by sorting the data set by each feature. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    /// <typeparam name="BoosterType"> Booster type. </typeparam>
    template <typename LossFunctionType, typename BoosterType>
    class SortingForestTrainer : public ForestTrainer<predictors::SingleElementThresholdPredictor, predictors::ConstantPredictor, BoosterType>
    {
    public:
        /// <summary> Constructs an instance of SortingForestTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="booster"> The booster. </param>
        /// <param name="parameters"> Training Parameters. </param>
        SortingForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const SortingForestTrainerParameters& parameters);

        using SplitRuleType = predictors::SingleElementThresholdPredictor;
        using EdgePredictorType = predictors::ConstantPredictor;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplitCandidate;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::SplittableNodeId;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::NodeStats;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Range;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::Sums;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::TrainerMetadata;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::PredictorType;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::DataVectorType;
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::TrainerExampleType;

    protected:
        using ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::_dataset;
        SplitCandidate GetBestSplitRuleAtNode(SplittableNodeId nodeId, Range range, Sums sums) override;
        std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats) override;

    private:
        void SortNodeDataset(Range range, size_t featureIndex);
        double CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const;

        // member variables
        LossFunctionType _lossFunction;
    };

    /// <summary> Makes a simple forest trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="lossFunction"> The loss function. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A unique_ptr to a simple forest trainer. </returns>
    template <typename LossFunctionType, typename BoosterType>
    std::unique_ptr<ITrainer<predictors::SimpleForestPredictor>> MakeSortingForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const SortingForestTrainerParameters& parameters);
} // namespace trainers
} // namespace ell

#pragma region implementation

namespace ell
{
namespace trainers
{
    template <typename LossFunctionType, typename BoosterType>
    SortingForestTrainer<LossFunctionType, BoosterType>::SortingForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const SortingForestTrainerParameters& parameters) :
        ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>(booster, parameters),
        _lossFunction(lossFunction)
    {
    }

    template <typename LossFunctionType, typename BoosterType>
    auto SortingForestTrainer<LossFunctionType, BoosterType>::GetBestSplitRuleAtNode(SplittableNodeId nodeId, Range range, Sums sums) -> SplitCandidate
    {
        auto numFeatures = _dataset.NumFeatures();

        SplitCandidate bestSplitCandidate(nodeId, range, sums);

        for (size_t inputIndex = 0; inputIndex < numFeatures; ++inputIndex)
        {
            // sort the relevant rows of data set in ascending order by inputIndex
            SortNodeDataset(range, inputIndex);

            Sums sums0;

            // consider all thresholds
            double nextFeatureValue = _dataset[range.firstIndex].GetDataVector()[inputIndex];
            for (size_t rowIndex = range.firstIndex; rowIndex < range.firstIndex + range.size - 1; ++rowIndex)
            {
                // get friendly names
                double currentFeatureValue = nextFeatureValue;
                nextFeatureValue = _dataset[rowIndex + 1].GetDataVector()[inputIndex];

                // increment sums
                sums0.Increment(_dataset[rowIndex].GetMetadata().weak);

                // only split between rows with different feature values
                if (currentFeatureValue == nextFeatureValue)
                {
                    continue;
                }

                // compute sums1 and gain
                auto sums1 = sums - sums0;
                double gain = CalculateGain(sums, sums0, sums1);

                // find gain maximizer
                if (gain > bestSplitCandidate.gain)
                {
                    bestSplitCandidate.gain = gain;
                    bestSplitCandidate.splitRule = SplitRuleType{ inputIndex, 0.5 * (currentFeatureValue + nextFeatureValue) };
                    bestSplitCandidate.ranges.SplitChildRange(0, rowIndex - range.firstIndex + 1);
                    bestSplitCandidate.stats.SetChildSums({ sums0, sums1 });
                }
            }
        }
        return bestSplitCandidate;
    }

    template <typename LossFunctionType, typename BoosterType>
    auto SortingForestTrainer<LossFunctionType, BoosterType>::GetEdgePredictors(const NodeStats& nodeStats) -> std::vector<EdgePredictorType>
    {
        double output = nodeStats.GetTotalSums().GetMeanLabel();
        double output0 = nodeStats.GetChildSums(0).GetMeanLabel() - output;
        double output1 = nodeStats.GetChildSums(1).GetMeanLabel() - output;
        return std::vector<EdgePredictorType>{ output0, output1 };
    }

    template <typename LossFunctionType, typename BoosterType>
    void SortingForestTrainer<LossFunctionType, BoosterType>::SortNodeDataset(Range range, size_t inputIndex)
    {
        _dataset.Sort([inputIndex](const data::Example<DataVectorType, TrainerMetadata>& example) { return example.GetDataVector()[inputIndex]; },
                      range.firstIndex,
                      range.size);
    }

    template <typename LossFunctionType, typename BoosterType>
    double SortingForestTrainer<LossFunctionType, BoosterType>::CalculateGain(const Sums& sums, const Sums& sums0, const Sums& sums1) const
    {
        if (sums0.sumWeights == 0 || sums1.sumWeights == 0)
        {
            return 0;
        }

        return sums0.sumWeights * _lossFunction.BregmanGenerator(sums0.sumWeightedLabels / sums0.sumWeights) +
               sums1.sumWeights * _lossFunction.BregmanGenerator(sums1.sumWeightedLabels / sums1.sumWeights) -
               sums.sumWeights * _lossFunction.BregmanGenerator(sums.sumWeightedLabels / sums.sumWeights);
    }

    template <typename LossFunctionType, typename BoosterType>
    std::unique_ptr<ITrainer<predictors::SimpleForestPredictor>> MakeSortingForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const SortingForestTrainerParameters& parameters)
    {
        return std::make_unique<SortingForestTrainer<LossFunctionType, BoosterType>>(lossFunction, booster, parameters);
    }
} // namespace trainers
} // namespace ell

#pragma endregion implementation
