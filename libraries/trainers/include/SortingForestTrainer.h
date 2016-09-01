////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ForestTrainer.h"
#include "LogitBooster.h"

// predictors
#include "SingleElementThresholdPredictor.h"
#include "ConstantPredictor.h"

namespace trainers
{
    /// <summary> Parameters for the sorting forest trainer. </summary>
    struct SortingForestTrainerParameters : public virtual ForestTrainerParameters
    {
    };

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
        using typename ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::ForestTrainerExample;

    protected:
        using ForestTrainer<SplitRuleType, EdgePredictorType, BoosterType>::_dataset;
        virtual SplitCandidate GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums) override;
        virtual std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats) override;

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
    template<typename LossFunctionType, typename BoosterType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeSortingForestTrainer(const LossFunctionType& lossFunction, const BoosterType& booster, const SortingForestTrainerParameters& parameters);
}

#include "../tcc/SortingForestTrainer.tcc"
