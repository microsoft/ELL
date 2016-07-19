////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleForestTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ForestTrainer.h"

namespace trainers
{
    template <typename LossFunctionType> 
    class SimpleForestTrainer : public ForestTrainer<LossFunctionType>
    {
    public:
        using ForestTrainer<LossFunctionType>::ForestTrainer;

        using SplitRuleType = predictors::SingleInputThresholdRule;
        using EdgePredictorType = predictors::ConstantPredictor;

    protected:
        virtual SplitCandidate GetBestSplitCandidateAtNode(SplittableNodeId nodeId, Range range, Sums sums) override;

        virtual std::vector<EdgePredictorType> GetEdgePredictors(const NodeStats& nodeStats) override;


    };

    /// <summary> Makes a simple forest trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="lossFunction"> The loss function. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A unique_ptr to a simple forest trainer. </returns>
    template<typename LossFunctionType>
    std::unique_ptr<IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeSimpleForestTrainer(const LossFunctionType& lossFunction, const ForestTrainerParameters& parameters);
}

#include "../tcc/SimpleForestTrainer.tcc"
