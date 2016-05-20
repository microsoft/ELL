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

// evaluators
#include "IEvaluator.h"

namespace trainers
{
    /// <summary> Parameters for the bagging incremental trainer. </summary>
    struct BaggingIncrementalTrainerParameters
    {
        uint64_t numIterations = 1;
        uint64_t bagSize = 0;
        std::string dataPermutationRandomSeed = "123456";
    };

    /// <summary> Implements a bagging incremental trainer. </summary>
    ///
    /// <typeparam name="BasePredictorType"> Type of the base predictor type. </typeparam>
    template <typename BasePredictorType>
    class BaggingIncrementalTrainer : public IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>
    {
    public:
        using PredictorType = predictors::EnsemblePredictor<BasePredictorType>;

        BaggingIncrementalTrainer() = delete;

        /// <summary> Constructs an instance of SingleEpochTrainer. </summary>
        ///
        /// <param name="incrementalTrainer"> [in,out] The stateful trainer. </param>
        BaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer, const BaggingIncrementalTrainerParameters& baggingParameters, std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator);

        /// <summary> Updates the state of the trainer by performing a learning iteration. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const PredictorType> GetPredictor() const override { return _ensemble; }

    private:
        std::unique_ptr<ITrainer<BasePredictorType>> _trainer;
        BaggingIncrementalTrainerParameters _baggingParameters;
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> _evaluator;
        std::shared_ptr<PredictorType> _ensemble;
        std::default_random_engine _random;
    };

    /// <summary> Makes a bagging incremental trainer. </summary>
    ///
    /// <typeparam name="BasePredictorType"> Type of base predictor used to build the ensemble. </typeparam>
    /// <param name="trainer"> [in,out] The incremental trainer. </param>
    /// <param name="baggingParameters"> Bagging parameters. </param>
    ///
    /// <returns> A unique_ptr to a multi-epoch trainer. </returns>
    template <typename BasePredictorType>
    std::unique_ptr<IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingIncrementalTrainer(
        std::unique_ptr<ITrainer<BasePredictorType>>&& trainer,
        const BaggingIncrementalTrainerParameters& baggingParameters,
        std::shared_ptr<evaluators::IEvaluator<predictors::EnsemblePredictor<BasePredictorType>>> evaluator = nullptr);
}

#include "../tcc/BaggingIncrementalTrainer.tcc"
