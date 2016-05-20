////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IEvaluator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"

// stl
#include <iostream>

namespace evaluators
{
    /// <summary> Interface to an evaluator. </summary>
    ///
    /// <typeparam name="PredictorType"> The predictor type. </typeparam>
    template <typename PredictorType>
    class IEvaluator 
    {
    public:

        virtual ~IEvaluator() = default;

        /// <summary> Runs the given predictor on the evaluation set, invokes each of the aggregators on the output, and logs the result. </summary>
        ///
        /// <param name="predictor"> The predictor. </param>
        virtual void Evaluate(const PredictorType& predictor) = 0;

        /// <summary> Prints the logged evaluations to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        virtual void Print(std::ostream& os) const = 0;
    };
}