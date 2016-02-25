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
#include "SharedLinearBinaryPredictor.h"

// linear
#include "DoubleVector.h"

namespace optimization
{
    /// <summary>
    /// Implements the Averaged Stochstic Gradient Descent algorithm on an L2 regularized empirical
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
        /// <typeparam name="ExampleIteratorType"> Type of the example iterator type. </typeparam>
        /// <typeparam name="LossFunctionType"> Type of the loss function type. </typeparam>
        /// <param name="data_iter"> [in,out] The data iterator. </param>
        /// <param name="loss_function"> The loss function. </param>
        /// <param name="l2Regularization"> The 2 regularization. </param>
        template<typename ExampleIteratorType, typename LossFunctionType>
        void Update(ExampleIteratorType& data_iter, const LossFunctionType& loss_function, double l2Regularization);

        /// <summary> \returns The averaged predictor. </summary>
        ///
        /// <returns> The predictor. </returns>
        const predictors::SharedLinearBinaryPredictor GetPredictor() const;
            
    private:
        uint64 _total_iterations;
        linear::DoubleVector _w;
        double _b;
        predictors::SharedLinearBinaryPredictor _predictor;
    };
}

#include "../tcc/AsgdOptimizer.tcc"
