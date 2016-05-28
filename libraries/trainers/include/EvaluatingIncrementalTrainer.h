////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EvaluatingIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIncrementalTrainer.h"
#include "Evaluator.h"

namespace trainers
{
    /// <summary> An evaluating incremental trainer, which wraps another incremental trainer and performs an evaluation after each update. </summary>
    ///
    /// <typeparam name="PredictorType"> The predictor type. </typeparam>
    template <typename PredictorType>
    class EvaluatingIncrementalTrainer : public IIncrementalTrainer<PredictorType>
    {
    public:

        /// <summary> Constructs an instance of EvaluatingIncrementalTrainer. </summary>
        ///
        /// <param name="internalTrainer"> An incremental trainer. </param>
        /// <param name="evaluator"> An evaluator. </param>
        EvaluatingIncrementalTrainer(
            std::unique_ptr<IIncrementalTrainer<PredictorType>>&& internalTrainer, 
            std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator);

        /// <summary> Updates the state of the trainer by performing a learning iteration. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const PredictorType> GetPredictor() const override { return _internalTrainer->GetPredictor(); }

        /// <summary> Gets a const reference to the evaluator. </summary>
        ///
        /// <returns> A shared pointer to the evaluator. </returns>
        virtual const std::shared_ptr<const evaluators::IEvaluator<PredictorType>> GetEvaluator() const { return _evaluator; }

    private:
        std::unique_ptr<IIncrementalTrainer<PredictorType>> _internalTrainer;
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> _evaluator;
    };

    /// <summary> Makes an evaluating trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="internalTrainer"> An incremental trainer. </param>
    /// <param name="evaluator"> An evaluator. </param>
    ///
    /// <returns> A unique_ptr to an evaluating trainer. </returns>
    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeEvaluatingIncrementalTrainer(
        std::unique_ptr<IIncrementalTrainer<PredictorType>>&& internalTrainer, 
        std::shared_ptr<evaluators::IEvaluator<PredictorType>>&& evaluator);
}

#include "../tcc/EvaluatingIncrementalTrainer.tcc"