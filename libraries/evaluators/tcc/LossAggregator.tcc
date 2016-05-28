////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LossAggregator.tcc (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace evaluators
{
    template<typename LossFunctionType>
    std::vector<double> evaluators::LossAggregator<LossFunctionType>::Value::GetValues() const
    {
        return{ GetMeanLoss() };
    }

    template<typename LossFunctionType>
    double evaluators::LossAggregator<LossFunctionType>::Value::GetMeanLoss() const
    {
        return sumWeights == 0.0 ? 0.0 : sumWeightedLosses / sumWeights;
    }

    template<typename LossFunctionType>
    LossAggregator<LossFunctionType>::LossAggregator(LossFunctionType lossFunction) : _lossFunction(std::move(lossFunction))
    {}

    template<typename LossFunctionType>
    void LossAggregator<LossFunctionType>::Update(double prediction, double label, double weight)
    {
        double loss = _lossFunction.Evaluate(prediction, label);
        _value.sumWeights += weight;
        _value.sumWeightedLosses += weight * loss;
    }

    template<typename LossFunctionType>
    typename LossAggregator<LossFunctionType>::Value LossAggregator<LossFunctionType>::GetAndReset()
    {
        Value newValue;
        std::swap(_value, newValue);
        return newValue;
    }

    template <typename LossFunctionType>
    std::vector<std::string> LossAggregator<LossFunctionType>::GetValueNames() const 
    { 
        return {"MeanLoss"}; 
    }

    template <typename LossFunctionType>
    LossAggregator<LossFunctionType> MakeLossAggregator(LossFunctionType lossFunction)
    {
        return LossAggregator<LossFunctionType>(std::move(lossFunction));
    }
}