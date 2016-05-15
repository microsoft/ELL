////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingLearner.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ILearner.h"

// predictors
#include "EnsemblePredictor.h"

namespace trainers
{
    template <typename BasePredictorType>
    class BaggingLearner : public ILearner<predictors::EnsemblePredictor<BasePredictorType>>
    {
    public:
        BaggingLearner() = delete;

        /// <summary> Constructs an instance of SingleEpochTrainer. </summary>
        ///
        /// <param name="learner"> [in,out] The stateful trainer. </param>
        BaggingLearner(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer);

        /// <summary> Updates the state of the trainer by performing a learning iteration. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Returns the trained predictor and resets the trainer to its initial state. </summary>
        ///
        /// <returns> The current trained predictor. </returns>
        virtual predictors::EnsemblePredictor<BasePredictorType> Reset() override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A constant reference to the current predictor. </returns>
        virtual const predictors::EnsemblePredictor<BasePredictorType>& GetPredictor() const override;

    private:
        std::unique_ptr<ITrainer<BasePredictorType>> _trainer;
        predictors::EnsemblePredictor _ensemble;
    };

    template <typename BasePredictorType>
    std::unique_ptr<ILearner<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingLearner(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer);

    template <typename BasePredictorType>
    std::unique_ptr<ITrainer<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer);
}

#include "../tcc/BaggingLearner.tcc"
