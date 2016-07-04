////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleInputThresholdRule.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace predictors
{
    /// <summary> A split rule that compares a single feature to a threshold. </summary>
    class SingleInputThresholdRule
    {
    public:
        /// <summary> Constructs a single-input threshold rule. </summary>
        ///
        /// <param name="index"> Zero-based index of the input coordinate. </param>
        /// <param name="threshold"> The threshold. </param>
        SingleInputThresholdRule(size_t index, double threshold);

        /// <summary> Evaluates the split rule. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the input. </typeparam>
        /// <param name="dataVector"> The input vector. </param>
        ///
        /// <returns> The result of the split rule. </returns>
        template<typename RandomAccessVectorType>
        int Compute(const RandomAccessVectorType& inputVector) const;

        /// <summary> Returns the number of outputs (the max output value plus one). </summary>
        ///
        /// <returns> The number of outputs. </returns>
        size_t NumOutputs() const { return 2; }

    private:
        size_t _index;
        double _threshold;
    };
}

#include "../tcc/SingleInputThresholdRule.tcc"
