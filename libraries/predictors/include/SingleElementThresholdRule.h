////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleElementThresholdRule.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <iostream>

namespace predictors
{
    /// <summary> A split rule that compares a single feature to a threshold. </summary>
    class SingleElementThresholdRule
    {
    public:
        /// <summary> Constructs a single-element threshold rule. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// <param name="threshold"> The threshold. </param>
        SingleElementThresholdRule(size_t index = 0, double threshold = 0.0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "SingleElementThresholdRule"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const { return GetTypeName(); }

        /// <summary> Gets the index of the element used to define the rule. </summary>
        ///
        /// <returns> The element index. </returns>
        size_t GetElementIndex() const { return _index; }

        /// <summary> Gets the threshold. </summary>
        ///
        /// <returns> The threshold value. </returns>
        double GetThreshold() const { return _threshold; }

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

        /// <summary> Prints the rule to an output stream on a separate line. </summary>
        ///
        /// <param name="os"> The output stream. </param>
        ///  <param name="tabs"> The number of tabs. </param>
        void PrintLine(std::ostream& os, size_t tabs=0) const;

    private:
        size_t _index;
        double _threshold;
    };
}

#include "../tcc/SingleElementThresholdRule.tcc"
