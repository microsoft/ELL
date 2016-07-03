////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleInputThresholdRule.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "DenseDataVector.h"

namespace predictors
{
    /// <summary> A split rule that compares a single feature to a threshold. </summary>
    class SingleInputThresholdRule
    {
    public:

        /// <summary> The number of outputs (or, put another way, the max output plus one). </summary>
        constexpr static size_t NumOutputs = 2;

        /// <summary> Constructs a single-input threshold rule. </summary>
        ///
        /// <param name="inputIndex"> Zero-based index of the input coordinate. </param>
        /// <param name="threshold"> The threshold. </param>
        SingleInputThresholdRule(size_t inputIndex, double threshold);

        /// <summary> Evaluates the split rule. </summary>
        ///
        /// <param name="dataVector"> The data vector. </param>
        ///
        /// <returns> The result of the split rule. </returns>
        size_t operator()(const dataset::DoubleDataVector& dataVector) const;

        ///
        /// <returns> The total number of outputs. </returns>
        size_t NumOutputs() const { return 2; }

    private:
        size_t _inputIndex;
        double _threshold;
    };
}