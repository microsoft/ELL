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
    template<typename PredictorType, typename AggregatorTupleType>
    class Evaluator : public IEvaluator<PredictorType>
    {
    public:
        Evaluator(dataset::GenericRowDataset::Iterator exampleIterator, AggregatorTupleType aggregatorTuple);

        virtual void Evaluate(const PredictorType& predictor) override;

        virtual void Print(std::ostream& os) const override;

    private:
        template<std::size_t ...Is>
        void DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Is...>);

        template<std::size_t ...Is>
        void DispatchPrint(std::ostream& os, std::index_sequence<Is...>) const;

        dataset::GenericRowDataset _rowDataset;
        AggregatorTupleType _aggregatorTuple;
    };

    template<typename PredictorType, typename... AggregatorTupleType>
    std::unique_ptr<IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, AggregatorTupleType... aggregatorTuple);
}

#include "../tcc/Evaluator.tcc"