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

            double weight = example.GetWeight();
            double label = example.GetLabel();
            double prediction = predictor.Predict(example.GetDataVector());

            DispatchUpdate(prediction, label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            iterator.Next();
        }
        Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template<typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::Print(std::ostream& os) const
    {
        DispatchPrint(os, std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template<typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::EvaluateZero()
    {
        auto iterator = _rowDataset.GetIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetWeight();
            double label = example.GetLabel();

            DispatchUpdate(0.0, label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            iterator.Next();
        }
        Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template<typename PredictorType, typename... AggregatorTypes>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, AggregatorTypes...>::DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Is...>)
    {
        // Call X.Update() for each X in _aggregatorTuple
        auto dummy = {(std::get<Is>(_aggregatorTuple).Update(prediction, label, weight), 0)...}; 
    }

    template<typename PredictorType, typename... AggregatorTypes>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, AggregatorTypes...>::Aggregate(std::index_sequence<Is...>)
    {
        // Call X.GetAndReset() for each X in _aggregatorTuple
        auto valueTuple = std::make_tuple(std::get<Is>(_aggregatorTuple).GetAndReset()...);
        _valueTuples.push_back(std::move(valueTuple));
    }

    template<typename PredictorType, typename... AggregatorTypes>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, AggregatorTypes...>::DispatchPrint(std::ostream& os, std::index_sequence<Is...>) const
    {
        // print header
        std::vector<std::string> header {std::get<Is>(_aggregatorTuple).GetHeader()...};
        os << header[0];
        for(uint64_t i = 1; i<header.size(); ++i)
        {
            os << "\t" << header[i];
        }
        os << std::endl;

        for(const auto& valueTuple : _valueTuples)
        {
            // Call X.ToString() for each X in valueTuple
            std::vector<std::string> strings {std::get<Is>(valueTuple).ToString()...};

            os << strings[0];
            for(uint64_t i = 1; i<strings.size(); ++i)
            {
                os << "\t" << strings[i];
            }
            os << std::endl;
        }
    }

    template<typename PredictorType, typename... AggregatorTypes>
    std::shared_ptr<IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators)
    {
        return std::make_unique<Evaluator<PredictorType, AggregatorTypes...>>(exampleIterator, evaluatorParameters, aggregators...);
    }
}