////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     BinaryClassificationEvaluator.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AnyIterator.h"

#include "SupervisedExample.h"
#include "LinearPredictor.h"

// stl
#include <vector>
#include <ostream>

namespace utilities
{
    /// <summary> A binary classifier evaluator </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor type. </typeparam>
    /// <typeparam name="LossFunctionType"> Type of the loss function type. </typeparam>
    template<typename PredictorType, typename LossFunctionType>
    class BinaryClassificationEvaluator
    {
    public:

        /// <summary> The result of an evaluation. </summary>
        struct Result
        {
            double loss = 0;
            double error = 0;
        };
        
        BinaryClassificationEvaluator();

        /// <summary> Evaluates a binary classifier </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of the example iterator type. </typeparam>
        /// <param name="dataIterator"> [in,out] The data iterator. </param>
        /// <param name="predictor"> The predictor. </param>
        /// <param name="lossFunction"> The loss function. </param>
        template<typename ExampleIteratorType>
        void Evaluate(ExampleIteratorType& dataIterator, const PredictorType& predictor, const LossFunctionType& lossFunction);

        /// <summary> Evaluates a binary classifier </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of the example iterator type. </typeparam>
        /// <param name="dataIterator"> [in,out] The data iterator. </param>
        /// <param name="predictor"> The predictor. </param>
        /// <param name="lossFunction"> The loss function. </param>
        void Evaluate(utilities::AnyIterator<dataset::SupervisedExample>& dataIterator, const PredictorType& predictor, const LossFunctionType& lossFunction);

        /// <summary> Returns the most recent average weighted loss. </summary>
        ///
        /// <returns> The most recent loss. </returns>
        double GetLastLoss() const;

        /// <summary> Returns the most recent weighted error rate. </summary>
        ///
        /// <returns> The most recent error rate. </returns>
        double GetLastError() const;

        /// <summary> Prints losses and errors to an ostream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Print(std::ostream& os) const;

    private:
        std::vector<Result> _evals;
    };

    template <typename PredictorType, typename LossFunctionType>
    std::ostream& operator<<(std::ostream& os, const BinaryClassificationEvaluator<PredictorType, LossFunctionType>& evaluation);
}

#include "../tcc/BinaryClassificationEvaluator.tcc"
