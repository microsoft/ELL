////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EvaluatingIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Evaluator.h"
#include "IIncrementalTrainer.h"

// stl
#include <memory>

namespace emll
{
namespace trainers
{
    /// <summary>
    /// Implements an evaluating incremental trainer. This trainer contains another incremental
    /// trainer and an evaluator, and performs an evaluation after each update.
    /// </summary>
    ///
    /// <typeparam name="PredictorType"> The predictor type. </typeparam>
    template <typename PredictorType>
    class EvaluatingIncrementalTrainer : public IIncrementalTrainer<PredictorType>
    {
    public:
        typedef IIncrementalTrainer<PredictorType> InternalTrainerType;
        typedef evaluators::IEvaluator<PredictorType> EvaluatorType;

        /// <summary> Constructs an instance of EvaluatingIncrementalTrainer. </summary>
        ///
        /// <param name="internalTrainer"> An incremental trainer. </param>
        /// <param name="evaluator"> An evaluator. </param>
        EvaluatingIncrementalTrainer(std::unique_ptr<InternalTrainerType>&& internalTrainer, std::shared_ptr<EvaluatorType> evaluator);

        /// <summary> Updates the state of the trainer by performing a learning iteration. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::AutoSupervisedDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const PredictorType> GetPredictor() const override { return _internalTrainer->GetPredictor(); }

        /// <summary> Gets a const reference to the evaluator. </summary>
        ///
        /// <returns> A shared pointer to the evaluator. </returns>
        virtual const std::shared_ptr<const EvaluatorType> GetEvaluator() const { return _evaluator; }

    private:
        std::unique_ptr<InternalTrainerType> _internalTrainer;
        std::shared_ptr<EvaluatorType> _evaluator;
    };

    /// <summary> Makes an evaluating trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="internalTrainer"> An incremental trainer. </param>
    /// <param name="evaluator"> An evaluator. </param>
    ///
    /// <returns> A unique_ptr to an evaluating trainer. </returns>
    template <typename PredictorType>
    EvaluatingIncrementalTrainer<PredictorType> MakeEvaluatingIncrementalTrainer(
        std::unique_ptr<IIncrementalTrainer<PredictorType>>&& internalTrainer,
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator);
}
}

#include "../tcc/EvaluatingIncrementalTrainer.tcc"
