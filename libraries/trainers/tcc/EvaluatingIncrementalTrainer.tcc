////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EvaluatingIncrementalTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EvaluatingIncrementalTrainer.h"

// stl
#include <utility>

namespace evaluators
{
    template<typename PredictorType>
    EvaluatingIncrementalTrainer<PredictorType>::EvaluatingIncrementalTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& internalTrainer, std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator) : _internalTrainer(std::move(internalTrainer)), _evaluator(evaluator)
    {}

    template<typename PredictorType>
    void EvaluatingIncrementalTrainer<PredictorType>::Update(dataset::GenericRowDataset::Iterator&& exampleIterator)
    {
        _internalTrainer->Update(std::forward<dataset::GenericRowDataset::Iterator>(exampleIterator));
        _evaluator->Evaluate(*_internalTrainer->GetPredictor());
    }

    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeEvaluatingIncrementalTrainer(
        std::unique_ptr<IIncrementalTrainer<PredictorType>>&& internalTrainer,
        std::shared_ptr<evaluators::IEvaluator<PredictorType>>&& evaluator)
    {
        return EvaluatingIncrementalTrainer<PredictorType>(std::move(internalTrainer), std::forward<std::shared_ptr<evaluators::IEvaluator<PredictorType>>>(evaluator));
    }
}