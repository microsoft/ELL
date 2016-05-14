////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingLearner.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ILearner.h"

namespace trainers
{
    template <typename PredictorType>
    class BaggingLearner : public ILearner<PredictorType>
    {
    public:
        BaggingLearner() = delete;

        /// <summary> Constructs an instance of SingleEpochTrainer. </summary>
        ///
        /// <param name="learner"> [in,out] The stateful trainer. </param>
        BaggingLearner(std::unique_ptr<ITrainer<PredictorType>>&& trainer);

        /// <summary> Updates the state of the trainer by performing a learning iteration. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Returns the trained predictor and resets the trainer to its initial state. </summary>
        ///
        /// <returns> The current trained predictor. </returns>
        virtual PredictorType Reset() override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A constant reference to the current predictor. </returns>
        virtual const PredictorType& GetPredictor() const override;

    private:
        std::unique_ptr<ITrainer<PredictorType>> _trainer;
    };

    template <typename PredictorType>
    std::unique_ptr<ILearner<PredictorType>> MakeBaggingLearner(std::unique_ptr<ITrainer<PredictorType>>&& trainer);

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeBaggingTrainer(std::unique_ptr<ITrainer<PredictorType>>&& trainer);
}