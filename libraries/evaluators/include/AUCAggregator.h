////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AUCAggregator.h (evaluators)
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
    /// <summary> An evaluation aggregator that computes AUC. </summary>
    class AUCAggregator
    {
    public:

        struct Value
        {
            double auc = 0.0;

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

        /// <summary> Gets a header that describes Value::ToString(). </summary>
        ///
        /// <returns> The header string. </returns>
        std::string GetHeader() const { return "AUC"; }

    private:

        struct Aggregate
        {
            double prediction;
            double label;
            double weight;

            bool operator<(const Aggregate& other) const;
        };

        Value _value;
        std::vector<Aggregate> _aggregates;
    };
}