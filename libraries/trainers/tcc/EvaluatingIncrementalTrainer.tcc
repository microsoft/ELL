////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EvaluatingIncrementalTrainer.tcc (trainers)
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
    EvaluatingIncrementalTrainer<PredictorType>::EvaluatingIncrementalTrainer(
        std::unique_ptr<InternalTrainerType>&& internalTrainer,
        std::shared_ptr<EvaluatorType> evaluator)
        : _internalTrainer(std::move(internalTrainer)), _evaluator(evaluator)
    {
        assert(_internalTrainer != nullptr);
        assert(_evaluator != nullptr);
    }

    template <typename PredictorType>
    void EvaluatingIncrementalTrainer<PredictorType>::Update(const data::AnyDataset& anyDataset)
    {
        _internalTrainer->Update(anyDataset);
        _evaluator->Evaluate(_internalTrainer->GetPredictor());
    }

    template <typename PredictorType>
    EvaluatingIncrementalTrainer<PredictorType> MakeEvaluatingIncrementalTrainer(
        std::unique_ptr<ITrainer<PredictorType>>&& internalTrainer,
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator)
    {
        return EvaluatingIncrementalTrainer<PredictorType>(std::move(internalTrainer), evaluator);
    }
}
}
