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
    evaluators::LossAggregator<LossFunctionType>::LossAggregator(const LossFunctionType & lossFunction) : _lossFunction(lossFunction)
    {}

    template<typename LossFunctionType>
    void evaluators::LossAggregator<LossFunctionType>::Update(double prediction, double label, double weight)
    {
        double loss = _lossFunction.Evaluate(prediction, label);
        _sumWeights += weight;
        _sumWeightedLoss += weight * loss;
    }

}