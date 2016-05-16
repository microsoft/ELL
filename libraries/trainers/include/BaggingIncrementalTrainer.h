////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIncrementalTrainer.h"
#include "ITrainer.h"

// predictors
#include "EnsemblePredictor.h"

namespace trainers
{
    template <typename BasePredictorType>
    class BaggingIncrementalTrainer : public IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>
    {
    public:
        BaggingIncrementalTrainer() = delete;

        /// <summary> Constructs an instance of SingleEpochTrainer. </summary>
        ///
        /// <param name="learner"> [in,out] The stateful trainer. </param>
        BaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer);

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
    std::unique_ptr<BaggingIncrementalTrainer<BasePredictorType>> MakeBaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer);

    template <typename BasePredictorType>
    std::unique_ptr<BaggingIncrementalTrainer<BasePredictorType>> MakeBaggingTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer);
}

#include "../tcc/BaggingIncrementalTrainer.tcc"
