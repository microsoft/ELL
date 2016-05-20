////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryErrorAggregator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <string>
namespace evaluators
{
    /// <summary> An evaluation aggregator that computes a binary confusion matrix. </summary>
    class BinaryErrorAggregator
    {
    public:

        struct Value
        {
            double sumWeightedTruePositives = 0.0;
            double sumWeightedTrueNegatives = 0.0;
            double sumWeightedFalsePositives = 0.0;
            double sumWeightedFalseNegatives = 0.0;

            /// <summary> Convert this object into a string. </summary>
            ///
            /// <returns> A std::string that represents this object. </returns>
            std::string ToString() const;
        };

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
        Value _value;
    };
}