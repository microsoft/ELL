////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LossAggregator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>

namespace evaluators
{
    /// <summary> An evaluation aggregator that computes mean loss. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type, requires Predict() function. </typeparam>
    template <typename LossFunctionType>
    class LossAggregator
    {
    public:

        struct Value
        {
            double sumWeights = 0.0;
            double sumWeightedLosses = 0.0;

            std::string ToString() const;
        };

        /// <summary> Constructs an instance of LossAggregator with a given loss. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        LossAggregator(const LossFunctionType& lossFunction);

        /// <summary> Updates this aggregator. </summary>
        ///
        /// <param name="prediction"> The real valued prediction. </param>
        /// <param name="label"> The label. </param>
        /// <param name="weight"> The weight. </param>
        void Update(double prediction, double label, double weight);

        /// <summary> Returns the current value and resets the aggregator to its initial state. </summary>
        ///
        /// <returns> The current value. </returns>
        Value GetAndReset();

    private:
        LossFunctionType _lossFunction;
        Value _value;
    };
}

#include "../tcc/LossAggregator.tcc"
