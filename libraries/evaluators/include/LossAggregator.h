////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LossAggregator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

namespace ell
{
namespace evaluators
{
    /// <summary> An evaluation aggregator that computes mean loss. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type, requires Predict() function. </typeparam>
    template <typename LossFunctionType>
    class LossAggregator
    {
    public:
        /// <summary> Constructs an instance of LossAggregator with a given loss. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        LossAggregator(LossFunctionType lossFunction);

        /// <summary> Updates this aggregator. </summary>
        ///
        /// <param name="prediction"> The real valued prediction. </param>
        /// <param name="label"> The label. </param>
        /// <param name="weight"> The weight. </param>
        void Update(double prediction, double label, double weight);

        /// <summary> Returns the current value. </summary>
        ///
        /// <returns> The current value. </returns>
        std::vector<double> GetResult() const;

        /// <summary> Resets the aggregator to its initial state. </summary>
        void Reset();

        /// <summary> Gets a header that describes the values of this aggregator. </summary>
        ///
        /// <returns> The header string vector. </returns>
        std::vector<std::string> GetValueNames() const;

    private:
        LossFunctionType _lossFunction;
        double _sumWeights = 0.0;
        double _sumWeightedLosses = 0.0;
    };

    template <typename LossFunctionType>
    LossAggregator<LossFunctionType> MakeLossAggregator(LossFunctionType lossFunction);
} // namespace evaluators
} // namespace ell

#pragma region implementation

namespace ell
{
namespace evaluators
{
    template <typename LossFunctionType>
    LossAggregator<LossFunctionType>::LossAggregator(LossFunctionType lossFunction) :
        _lossFunction(std::move(lossFunction))
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
} // namespace evaluators
} // namespace ell

#pragma endregion implementation
