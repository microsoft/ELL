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
    /// <summary> Parameters for the bagging incremental trainer. </summary>
    struct BaggingIncrementalTrainerParameters
    {
        uint64_t numIterations = 1;
    };

    template <typename BasePredictorType>
    class BaggingIncrementalTrainer : public IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>
    {
    public:
        BaggingIncrementalTrainer() = delete;

        /// <summary> Constructs an instance of SingleEpochTrainer. </summary>
        ///
        /// <param name="incrementalTrainer"> [in,out] The stateful trainer. </param>
        BaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer, const BaggingIncrementalTrainerParameters& parameters);

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

        virtual const utilities::IBinaryClassificationEvaluator<predictors::EnsemblePredictor<BasePredictorType>>* GetEvaluator() const override { return nullptr; }

    private:
        std::unique_ptr<ITrainer<BasePredictorType>> _trainer;
        BaggingIncrementalTrainerParameters _parameters;
        predictors::EnsemblePredictor _ensemble;
    };

    template <typename BasePredictorType>
    std::unique_ptr<IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer);
}

#include "../tcc/BaggingIncrementalTrainer.tcc"
