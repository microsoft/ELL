////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Evaluator.tcc (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace evaluators
{
    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    Evaluator<PredictorType, EvaluationAggregatorTupleType>::Evaluator(EvaluationAggregatorTupleType&& evaluationAggregatorTuple, dataset::GenericRowDataset::Iterator exampleIterator) 
        : _evaluationAggregatorTuple(std::move(evaluationAggregatorTuple)), _rowDataset(exampleIterator) {}

    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    void Evaluator<PredictorType, EvaluationAggregatorTupleType>::Evaluate(const PredictorType& predictor)
    {
        auto iterator = _rowDataset.GetIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();
            DispatchUpdate(predictor, example, std::make_index_sequence<std::tuple_size<decltype(_evaluationAggregatorTuple)>::value>());
            iterator.Next();
        }
    }

    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, EvaluationAggregatorTupleType>::DispatchUpdate(const PredictorType& predictor, const dataset::GenericSupervisedExample& example, std::index_sequence<Is...>)
    {
        double weight = example.GetWeight();
        double label = example.GetLabel();
        double prediction = predictor.Predict(example.GetDataVector());

        // Call X.Update() for each X in _evaluationAggregatorTuple
        auto dummy = { (std::get<Is>(_evaluationAggregatorTuple).Update(prediction, label, weight), 0)... }; // OMG!
    }
}