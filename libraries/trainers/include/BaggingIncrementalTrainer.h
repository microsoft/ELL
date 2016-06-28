////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIncrementalTrainer.h"
#include "IBlackBoxTrainer.h"

// predictors
#include "EnsemblePredictor.h"

// evaluators
#include "IncrementalEvaluator.h"

namespace trainers
{
    /// <summary> Parameters for the bagging incremental trainer. </summary>
    struct BaggingIncrementalTrainerParameters
    {
        uint64_t numIterations = 1;
        uint64_t bagSize = 0;
        std::string dataPermutationRandomSeed = "123456";
    };

    /// <summary>
    /// Implements a bagging incremental trainer. A bagging trainer runs a base trainer on random
    /// subsets of the training data and averages the resulting predictors to create an ensemble.
    /// </summary>
    ///
    /// <typeparam name="BasePredictorType"> Type of the base predictor type. </typeparam>
    template <typename BasePredictorType>
    class BaggingIncrementalTrainer : public IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>
    {
    public:
        typedef predictors::EnsemblePredictor<BasePredictorType> EnsembleType;
        typedef IBlackBoxTrainer<BasePredictorType> BaseTrainerType;
        typedef evaluators::IIncrementalEvaluator<BasePredictorType> EvaluatorType;

        BaggingIncrementalTrainer() = delete;

        /// <summary> Constructs an instance of BaggingIncrementalTrainer. </summary>
        ///
        /// <param name="trainer"> A base trainer. </param>
        /// <param name="baggingParameters"> Bagging paramters. </param>
        /// <param name="evaluator"> An optional evaluator, or nullptr. </param>
        BaggingIncrementalTrainer(std::unique_ptr<BaseTrainerType>&& baseTrainer, 
            const BaggingIncrementalTrainerParameters& baggingParameters, 
            std::shared_ptr<EvaluatorType> evaluator);

        /// <summary> Updates the state of the trainer by performing a learning iteration. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const EnsembleType> GetPredictor() const override { return _ensemble; }

    private:
        std::unique_ptr<BaseTrainerType> _baseTrainer;
        BaggingIncrementalTrainerParameters _baggingParameters;
        std::shared_ptr<EvaluatorType> _evaluator;
        std::shared_ptr<EnsembleType> _ensemble;
        std::default_random_engine _random;
    };

    /// <summary> Makes a bagging incremental trainer. </summary>
    ///
    /// <typeparam name="BasePredictorType"> Type of base predictor used to build the ensemble. </typeparam>
    /// <param name="trainer"> A trainer. </param>
    /// <param name="baggingParameters"> Bagging paramters. </param>
    /// <param name="evaluator"> An optional evaluator. </param>
    ///
    /// <returns> A unique_ptr to a multi-epoch trainer. </returns>
    template <typename BasePredictorType>
    std::unique_ptr<IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingIncrementalTrainer(
        std::unique_ptr<IBlackBoxTrainer<BasePredictorType>>&& baseTrainer,
        const BaggingIncrementalTrainerParameters& baggingParameters,
        std::shared_ptr<evaluators::IIncrementalEvaluator<BasePredictorType>> evaluator = nullptr);
}

#include "../tcc/BaggingIncrementalTrainer.tcc"
