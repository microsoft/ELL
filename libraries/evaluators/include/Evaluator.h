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
        Evaluator(dataset::GenericRowDataset::Iterator exampleIterator, EvaluationAggregatorTupleType evaluationAggregatorTuple);

        virtual void Evaluate(const PredictorType& predictor) override;

        virtual void Print(std::ostream& os) const override;

    private:

        template<std::size_t ...Is>
        void DispatchUpdate(const PredictorType& predictor, const dataset::GenericSupervisedExample& example, std::index_sequence<Is...>);

        template<std::size_t ...Is>
        void DispatchPrint(std::ostream& os, std::index_sequence<Is...>) const;

        dataset::GenericRowDataset _rowDataset;
        EvaluationAggregatorTupleType _evaluationAggregatorTuple;
    };

    template<typename PredictorType, typename... EvaluationAggregatorTupleType>
    std::unique_ptr<IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, EvaluationAggregatorTupleType... evaluationAggregatorTuple);

    //template<typename PredictorType, typename... AggregatorTypes>
    //Evaluator<PredictorType, std::tuple<AggregatorTypes...>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, AggregatorTypes... evaluationAggregatorTuple);
}

#include "../tcc/Evaluator.tcc"