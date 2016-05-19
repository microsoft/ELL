////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Evaluator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IEvaluator.h"

// dataset
#include "RowDataset.h"
#include "SupervisedExample.h"

// stl
#include <memory>

namespace evaluators
{
    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    class Evaluator : public IEvaluator<PredictorType>
    {
    public:
        Evaluator(EvaluationAggregatorTupleType&& evaluationAggregatorTuple, dataset::GenericRowDataset::Iterator exampleIterator);

        virtual void Evaluate(const PredictorType& predictor) override;


    private:

        template<std::size_t ...Is>
        void DispatchUpdate(const PredictorType& predictor, const dataset::GenericSupervisedExample& example, std::index_sequence<Is...>);

        EvaluationAggregatorTupleType _evaluationAggregatorTuple;
        dataset::GenericRowDataset _rowDataset;
    };

    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    Evaluator<PredictorType, EvaluationAggregatorTupleType> MakeEvaluator(EvaluationAggregatorTupleType&& evaluationAggregatorTuple, dataset::GenericRowDataset::Iterator exampleIterator)
    {
        return Evaluator<PredictorType, EvaluationAggregatorTupleType>(std::move(evaluationAggregatorTuple), exampleIterator);
    }
}

#include "../tcc/Evaluator.tcc"