////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Evaluator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"

// stl
#include <memory>

namespace evaluators
{
    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    class Evaluator
    {
    public:
        Evaluator(EvaluationAggregatorTupleType&& evaluationAggregatorTuple, dataset::GenericRowDataset::Iterator exampleIterator) : _evaluationAggregatorTuple(std::move(evaluationAggregatorTuple)), _rowDataset(exampleIterator) {}

        void Evaluate();


    private:

        template<std::size_t ...Is>
        void Update(std::index_sequence<Is...>);




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