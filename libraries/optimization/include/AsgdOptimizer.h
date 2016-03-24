////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     AsgdOptimizer.h (optimization)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// predictors
#include "LinearPredictor.h"

// linear
#include "DoubleVector.h"

namespace optimization
{
    /// <summary>
    /// Implements the Averaged Stochastic Gradient Descent algorithm on an L2 regularized empirical
    /// loss.
    /// </summary>
    class AsgdOptimizer
    {
    public:

        /// <summary> Constructs the optimizer. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        AsgdOptimizer(uint64 dim);

        /// <summary> Performs a given number of learning iterations. </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of example iterator to use. </typeparam>
        /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
        /// <param name="exampleIterator"> [in,out] The data iterator. </param>
        /// <param name="numExamples"> The number of examples in the iterator. </param>
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="l2Regularization"> The L2 regularization parameter. </param>
        template<typename ExampleIteratorType, typename LossFunctionType>
        void Update(ExampleIteratorType& exampleIterator, uint64 numExamples, const LossFunctionType& lossFunction, double l2Regularization);

        /// <summary> Returns The averaged predictor. </summary>
        ///
        /// <returns> The averaged predictor. </returns>
        const predictors::LinearPredictor& GetPredictor() const;
            
    private:
        uint64 _total_iterations;
        linear::DoubleVector _w;
        double _b;
        predictors::LinearPredictor _predictor;
    };
}

#include "../tcc/AsgdOptimizer.tcc"
