////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EvaluatingTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"

#include <evaluators/include/Evaluator.h>

#include <memory>

namespace ell
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
    class EvaluatingTrainer : public ITrainer<PredictorType>
    {
    public:
        typedef ITrainer<PredictorType> InternalTrainerType;
        typedef evaluators::IEvaluator<PredictorType> EvaluatorType;

        /// <summary> Constructs an instance of EvaluatingTrainer. </summary>
        ///
        /// <param name="internalTrainer"> An incremental trainer. </param>
        /// <param name="evaluator"> An evaluator. </param>
        EvaluatingTrainer(std::unique_ptr<InternalTrainerType>&& internalTrainer, std::shared_ptr<EvaluatorType> evaluator);

        /// <summary> Sets the trainer's dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        void SetDataset(const data::AnyDataset& anyDataset) override;

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        void Update() override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A const reference to the current predictor. </returns>
        const PredictorType& GetPredictor() const override { return _internalTrainer->GetPredictor(); }

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
    EvaluatingTrainer<PredictorType> MakeEvaluatingTrainer(
        std::unique_ptr<ITrainer<PredictorType>>&& internalTrainer,
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator);
} // namespace trainers
} // namespace ell

#pragma region implementation

#include <utility>

namespace ell
{
namespace trainers
{
    template <typename PredictorType>
    EvaluatingTrainer<PredictorType>::EvaluatingTrainer(
        std::unique_ptr<InternalTrainerType>&& internalTrainer,
        std::shared_ptr<EvaluatorType> evaluator) :
        _internalTrainer(std::move(internalTrainer)),
        _evaluator(evaluator)
    {
        assert(_internalTrainer != nullptr);
        assert(_evaluator != nullptr);
    }

    template <typename PredictorType>
    void EvaluatingTrainer<PredictorType>::SetDataset(const data::AnyDataset& anyDataset)
    {
        _internalTrainer->SetDataset(anyDataset);
    }

    template <typename PredictorType>
    void EvaluatingTrainer<PredictorType>::Update()
    {
        _internalTrainer->Update();
        _evaluator->Evaluate(_internalTrainer->GetPredictor());
    }

    template <typename PredictorType>
    EvaluatingTrainer<PredictorType> MakeEvaluatingTrainer(
        std::unique_ptr<ITrainer<PredictorType>>&& internalTrainer,
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator)
    {
        return EvaluatingTrainer<PredictorType>(std::move(internalTrainer), evaluator);
    }
} // namespace trainers
} // namespace ell

#pragma endregion implementation
