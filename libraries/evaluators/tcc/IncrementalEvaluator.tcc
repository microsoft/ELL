////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IncrementalEvaluator.tcc (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace evaluators
{
    template<typename PredictorType, typename... AggregatorTypes>
    IncrementalEvaluator<PredictorType, AggregatorTypes...>::IncrementalEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators) :
        Evaluator<PredictorType, AggregatorTypes...>(exampleIterator, evaluatorParameters, aggregators...)
    {
        _predictions.resize(_rowDataset.NumExamples());
    }

    template<typename PredictorType, typename... AggregatorTypes>
    void IncrementalEvaluator<PredictorType, AggregatorTypes...>::IncrementalEvaluate(const PredictorType& weakPredictor)
    {
        ++_evaluateCounter;
        bool evaluate = _evaluateCounter % _evaluatorParameters.evaluationFrequency == 0 ? true : false;

        auto iterator = _rowDataset.GetIterator();
        uint64_t index = 0;

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetWeight();
            double label = example.GetLabel();
            double weakPrediction = weakPredictor.Predict(example.GetDataVector());
            _predictions[index] += weakPrediction;

            if (evaluate)
            {
                DispatchUpdate(_predictions[index], label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            }

            iterator.Next();
            ++index;
        }
        if (evaluate)
        {
            Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
        }
    }

    template<typename PredictorType, typename... AggregatorTypes>
    void IncrementalEvaluator<PredictorType, AggregatorTypes...>::Print(std::ostream& os) const
    {
        Evaluator<PredictorType, AggregatorTypes...>::Print(os);
    }

    template<typename PredictorType, typename... AggregatorTypes>
    std::shared_ptr<IIncrementalEvaluator<PredictorType>> MakeIncrementalEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators)
    {
        return std::make_unique<IncrementalEvaluator<PredictorType, AggregatorTypes...>>(exampleIterator, evaluatorParameters, aggregators...);
    }
}
