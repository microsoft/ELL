////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Evaluator.tcc (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryErrorAggregator.h"

namespace evaluators
{
    template<typename PredictorType, typename AggregatorTupleType>
    Evaluator<PredictorType, AggregatorTupleType>::Evaluator(dataset::GenericRowDataset::Iterator exampleIterator, AggregatorTupleType aggregatorTuple)
        : _rowDataset(exampleIterator), _aggregatorTuple(std::move(aggregatorTuple)) 
    {
        static_assert(std::tuple_size<AggregatorTupleType>::value > 0, "Evaluator must contains at least one aggregator");
    }

    template<typename PredictorType, typename AggregatorTupleType>
    void Evaluator<PredictorType, AggregatorTupleType>::Evaluate(const PredictorType& predictor)
    {
        auto iterator = _rowDataset.GetIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetWeight();
            double label = example.GetLabel();
            double prediction = predictor.Predict(example.GetDataVector());

            DispatchUpdate(prediction, label, weight, std::make_index_sequence<std::tuple_size<AggregatorTupleType>::value>());
            iterator.Next();
        }
    }

    template<typename PredictorType, typename AggregatorTupleType>
    void Evaluator<PredictorType, AggregatorTupleType>::Print(std::ostream& os) const
    {
        DispatchPrint(os, std::make_index_sequence<std::tuple_size<decltype(_aggregatorTuple)>::value>());
    }

    template<typename PredictorType, typename AggregatorTupleType>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, AggregatorTupleType>::DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Is...>)
    {
        // Call X.Update() for each X in _aggregatorTuple
        auto dummy = {(std::get<Is>(_aggregatorTuple).Update(prediction, label, weight), 0)...}; // OMG!
    }

    template<typename PredictorType, typename AggregatorTupleType>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, AggregatorTupleType>::DispatchPrint(std::ostream& os, std::index_sequence<Is...>) const
    {
        // Call X.ToString() for each X in _aggregatorTuple
        std::vector<std::string> strings {std::get<Is>(_aggregatorTuple).ToString()...};

        os << strings[0];
        for(uint64_t i=1; i<strings.size(); ++i)
        {
            os << "\t" << strings[i];
        }
        os << std::endl;
    }

    template<typename PredictorType, typename... AggregatorTupleType>
    std::unique_ptr<IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, AggregatorTupleType... aggregatorTuple)
    {
        return std::make_unique<Evaluator<PredictorType, std::tuple<AggregatorTupleType...>>>(exampleIterator, std::make_tuple(aggregatorTuple...));
    }
}