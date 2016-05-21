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
    void IncrementalEvaluator<PredictorType, AggregatorTypes...>::IncrementalEvaluate(const PredictorType& predictor)
    {
        Evaluator<PredictorType, AggregatorTypes...>::Evaluate(predictor); // TODO
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
