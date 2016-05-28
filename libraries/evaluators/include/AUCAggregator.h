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

            /// <summary> Returns a number between 0 and 1 that represents the goodness of this value. </summary>
            ///
            /// <returns> The goodness of this value. </returns>
            double GetGoodness() const { return auc; }

            /// <summary> Gets the vector of evaluation values that match the descriptions in GetValueNames(). </summary>
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
        std::vector<std::string> GetValueNames() const;

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