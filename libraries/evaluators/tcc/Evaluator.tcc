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
    template<typename PredictorType, typename... AggregatorTypes>
    Evaluator<PredictorType, AggregatorTypes...>::Evaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators)
        : _rowDataset(exampleIterator), _evaluatorParameters(evaluatorParameters), _aggregatorTuple(std::make_tuple(aggregators...))
    {
        static_assert(sizeof...(AggregatorTypes) > 0, "Evaluator must contains at least one aggregator");

        if(_evaluatorParameters.addZeroEvaluation)
        {
            EvaluateZero();
        }
    }

    template<typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::Evaluate(const PredictorType& predictor)
    {
        ++_evaluateCounter;
        if(_evaluateCounter % _evaluatorParameters.evaluationFrequency != 0)
        {
            return;
        }

        auto iterator = _rowDataset.GetIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetMetaData().weight;
            double label = example.GetMetaData().label;
            double prediction = predictor.Predict(example.GetDataVector());

            DispatchUpdate(prediction, label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            iterator.Next();
        }
        Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template<typename PredictorType, typename ...AggregatorTypes>
    double Evaluator<PredictorType, AggregatorTypes...>::GetGoodness() const
    {
        if (_values.size() == 0)
        {
            return 0.0;
        }
        return _values.back()[0][0];
    }

    template<typename T>
    void PrintVector(std::ostream& os, const std::vector<T>& v)
    {
        if(v.size() == 0) return;

        os << v[0];
        for(uint64_t j = 1; j<v.size(); ++j)
        {
            os << '\t' << v[j];
        }
    }

    template<typename T>
    std::vector<T> FlattenJaggedVector(const std::vector<std::vector<T>>& v)
    {
        std::vector<T> concat;
        auto iter = v.cbegin();
        auto end = v.end();
        while(iter != end)
        {
            concat.insert(concat.end(), iter->cbegin(), iter->cend());
            ++iter;
        }
        return concat;
    }

    template<typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::Print(std::ostream& os) const
    {
        auto originalPrecision = os.precision(6);
        auto originalFlags = os.setf(std::ios::fixed);
     
        PrintVector(os, FlattenJaggedVector(GetValueNames()));

        for(const auto& values : _values)
        {
            os << std::endl;
            PrintVector(os, FlattenJaggedVector(values));
        }
        
        os.setf(originalFlags);
        os.precision(originalPrecision);
    }

    template<typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::EvaluateZero()
    {
        auto iterator = _rowDataset.GetIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetMetaData().weight;
            double label = example.GetMetaData().label;

            DispatchUpdate(0.0, label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            iterator.Next();
        }
        Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template<typename PredictorType, typename... AggregatorTypes>
    template<std::size_t... Sequence>
    void Evaluator<PredictorType, AggregatorTypes...>::DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Sequence...>)
    {
        // Call (X.Update(), 0) for each X in _aggregatorTuple
        // The ',0' above is due to the fact that Update returns void
        auto dummy = {(std::get<Sequence>(_aggregatorTuple).Update(prediction, label, weight), 0)...}; 
    }

    template<typename PredictorType, typename... AggregatorTypes>
    template<std::size_t ...Sequence>
    void Evaluator<PredictorType, AggregatorTypes...>::Aggregate(std::index_sequence<Sequence...>)
    {
        // Call X.GetResult() for each X in _aggregatorTuple
        _values.push_back({std::get<Sequence>(_aggregatorTuple).GetResult()...});

        // Call X.Reset() for each X in _aggregatorTuple
        auto dummy = {(std::get<Sequence>(_aggregatorTuple).Reset(), 0)... };
    }

    template<typename PredictorType, typename... AggregatorTypes>
    std::vector<std::vector<std::string>> Evaluator<PredictorType, AggregatorTypes...>::GetValueNames() const
    {
        return DispatchGetValueNames(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template<typename PredictorType, typename... AggregatorTypes>
    template<std::size_t ...Sequence>
    std::vector<std::vector<std::string>> Evaluator<PredictorType, AggregatorTypes...>::DispatchGetValueNames(std::index_sequence<Sequence...>) const
    {
        return {std::get<Sequence>(_aggregatorTuple).GetValueNames()...};
    }

    template<typename PredictorType, typename... AggregatorTypes>
    std::shared_ptr<IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators)
    {
        return std::make_unique<Evaluator<PredictorType, AggregatorTypes...>>(exampleIterator, evaluatorParameters, aggregators...);
    }
}
