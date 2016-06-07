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
#include <string>
#include <vector>

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

            /// <summary> Returns a number between 0 and 1 that represents the goodness of this value. </summary>
            ///
            /// <returns> The goodness of this value. </returns>
            double GetGoodness() const { return GetMeanLoss(); }

            /// <summary> Gets the vector of evaluation values that match the descriptions in GetValueNames(). </summary>
            ///
            /// <returns> A vector of evaluation values. </returns>
            std::vector<double> GetValues() const;

        private:
            double GetMeanLoss() const;
        };

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
        Value Get() const { return _value; }

        /// <summary> Resets the aggregator to its initial state. </summary>
        void Reset();

        /// <summary> Gets a header that describes the values of this aggregator. </summary>
        ///
        /// <returns> The header string vector. </returns>
        std::vector<std::string> GetValueNames() const;

    private:
        LossFunctionType _lossFunction;
        Value _value;
    };

    template <typename LossFunctionType>
    LossAggregator<LossFunctionType> MakeLossAggregator(LossFunctionType lossFunction);
}

#include "../tcc/LossAggregator.tcc"
