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
    void Evaluator<PredictorType, EvaluationAggregatorTupleType>::Evaluate()
    {
        Update(std::make_index_sequence<std::tuple_size<decltype(_evaluationAggregatorTuple)>::value>());
    }

    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, EvaluationAggregatorTupleType>::Update(std::index_sequence<Is...>)
    {
        // Call X.Update() for each X in _evaluationAggregatorTuple
        auto dummy = { (std::get<Is>(_evaluationAggregatorTuple).Update(true), 0)... }; // OMG!
    }
}