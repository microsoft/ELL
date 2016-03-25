////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     BinaryClassificationEvaluator.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>
#include <iostream>

namespace utilities
{
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
        /// <typeparam name="PredictorType"> Type of the predictor type. </typeparam>
        /// <typeparam name="LossFunctionType"> Type of the loss function type. </typeparam>
        /// <param name="data_iter"> [in,out] The data iterator. </param>
        /// <param name="predictor"> The predictor. </param>
        /// <param name="loss_function"> The loss function. </param>
        template<typename ExampleIteratorType, typename PredictorType, typename LossFunctionType>
        void Evaluate(ExampleIteratorType& data_iter, const PredictorType& predictor, const LossFunctionType& loss_function);

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

    std::ostream& operator<<(std::ostream& os, const BinaryClassificationEvaluator& evaluation);
}

#include "../tcc/BinaryClassificationEvaluator.tcc"
