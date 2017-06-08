////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EvaluatingTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <utility>

namespace ell
{
namespace trainers
{
    template <typename PredictorType>
    EvaluatingTrainer<PredictorType>::EvaluatingTrainer(
        std::unique_ptr<InternalTrainerType>&& internalTrainer,
        std::shared_ptr<EvaluatorType> evaluator)
        : _internalTrainer(std::move(internalTrainer)), _evaluator(evaluator)
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
}
}
