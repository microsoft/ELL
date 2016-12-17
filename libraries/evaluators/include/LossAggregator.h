////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LossAggregator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
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
}
}

#include "../tcc/LossAggregator.tcc"
