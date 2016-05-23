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
#include <vector>

namespace evaluators
{
    /// <summary> An evaluation aggregator that computes a binary confusion matrix. </summary>
    class BinaryErrorAggregator
    {
    public:

        struct Value
        {
            double truePositives = 0.0;
            double trueNegatives = 0.0;
            double falsePositives = 0.0;
            double falseNegatives = 0.0;

            /// <summary> Gets the vector of evaluation values that match the descriptions in GetHeader(). </summary>
            ///
            /// <returns> A vector of evaluation values. </returns>
            std::vector<double> GetValues() const;
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

        /// <summary> Gets a header that describes the values of this aggregator. </summary>
        ///
        /// <returns> The header string vector. </returns>
        std::vector<std::string> GetHeader() const;

    private:
        Value _value;
    };
}