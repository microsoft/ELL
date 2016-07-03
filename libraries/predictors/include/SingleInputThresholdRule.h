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

        /// <summary> The number of outputs (or, put another way, the max output plus one). </summary>
        constexpr static size_t NumOutputs = 2;

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
        size_t Compute(const RandomAccessVectorType& inputVector) const;

    private:
        size_t _index;
        double _threshold;
    };
}

#include "../tcc/SingleInputThresholdRule.tcc"
