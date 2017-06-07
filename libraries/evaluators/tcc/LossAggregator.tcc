////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LossAggregator.tcc (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace evaluators
{
    template <typename LossFunctionType>
    LossAggregator<LossFunctionType>::LossAggregator(LossFunctionType lossFunction)
        : _lossFunction(std::move(lossFunction))
    {
    }

    template <typename LossFunctionType>
    void LossAggregator<LossFunctionType>::Update(double prediction, double label, double weight)
    {
        double loss = _lossFunction(prediction, label);
        _sumWeights += weight;
        _sumWeightedLosses += weight * loss;
    }

    template <typename LossFunctionType>
    std::vector<double> LossAggregator<LossFunctionType>::GetResult() const
    {
        double meanLoss = _sumWeights == 0.0 ? 0.0 : _sumWeightedLosses / _sumWeights;
        return { meanLoss };
    }

    template <typename LossFunctionType>
    void LossAggregator<LossFunctionType>::Reset()
    {
        _sumWeights = 0.0;
        _sumWeightedLosses = 0.0;
    }

    template <typename LossFunctionType>
    std::vector<std::string> LossAggregator<LossFunctionType>::GetValueNames() const
    {
        return { "MeanLoss" };
    }

    template <typename LossFunctionType>
    LossAggregator<LossFunctionType> MakeLossAggregator(LossFunctionType lossFunction)
    {
        return LossAggregator<LossFunctionType>(std::move(lossFunction));
    }
}
}
