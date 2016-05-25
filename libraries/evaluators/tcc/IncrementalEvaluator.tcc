////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IncrementalEvaluator.tcc (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace evaluators
{
    template<typename BasePredictorType, typename... AggregatorTypes>
    IncrementalEvaluator<BasePredictorType, AggregatorTypes...>::IncrementalEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators) :
        Evaluator<BasePredictorType, AggregatorTypes...>(exampleIterator, evaluatorParameters, aggregators...)
    {
        _predictions.resize(_rowDataset.NumExamples());
    }

    template<typename BasePredictorType, typename... AggregatorTypes>
    void IncrementalEvaluator<BasePredictorType, AggregatorTypes...>::IncrementalEvaluate(const BasePredictorType& basePredictor, double basePredictorWeight, double evaluationRescale)
    {
        ++_evaluateCounter;
        bool evaluate = _evaluateCounter % _evaluatorParameters.evaluationFrequency == 0 ? true : false;

        auto iterator = _rowDataset.GetIterator();
        uint64_t index = 0;

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double exampleWeight = example.GetWeight();
            double label = example.GetLabel();
            _predictions[index] += basePredictorWeight * basePredictor.Predict(example.GetDataVector());

            if (evaluate)
            {
                DispatchUpdate(_predictions[index] * evaluationRescale, label, exampleWeight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            }

            iterator.Next();
            ++index;
        }
        if (evaluate)
        {
            Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
        }
    }

    template<typename BasePredictorType, typename ...AggregatorTypes>
    double IncrementalEvaluator<BasePredictorType, AggregatorTypes...>::GetGoodness() const
    {
        if (_valueTuples.size() == 0)
        {
            return 0;
        }
        return std::get<0>(_valueTuples.back()).GetGoodness();
    }

    template<typename BasePredictorType, typename... AggregatorTypes>
    void IncrementalEvaluator<BasePredictorType, AggregatorTypes...>::Print(std::ostream& os) const
    {
        Evaluator<BasePredictorType, AggregatorTypes...>::Print(os);
    }

    template<typename BasePredictorType, typename... AggregatorTypes>
    std::shared_ptr<IIncrementalEvaluator<BasePredictorType>> MakeIncrementalEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators)
    {
        return std::make_unique<IncrementalEvaluator<BasePredictorType, AggregatorTypes...>>(exampleIterator, evaluatorParameters, aggregators...);
    }
}
