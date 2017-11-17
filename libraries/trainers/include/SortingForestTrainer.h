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

// predictors
#include "ConstantPredictor.h"
#include "SingleElementThresholdPredictor.h"

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
}
}

#include "../tcc/SortingForestTrainer.tcc"
