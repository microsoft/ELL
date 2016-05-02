////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AsgdOptimizer.h (optimization)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// predictors
#include "LinearPredictor.h"

// loss functions
#include "SquaredLoss.h"
#include "LogLoss.h"

// linear
#include "DoubleVector.h"

// dataset
#include "SupervisedExample.h"

// utilities
#include "AnyIterator.h"

// stl
#include <cstdint>

namespace optimization
{
    /// <summary>
    /// Implements the Averaged Stochastic Gradient Descent algorithm on an L2 regularized empirical
    /// loss.
    /// </summary>    
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    template <typename LossFunctionType>
    class AsgdOptimizer
    {
    public:
        /// <summary> Constructs the optimizer. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="l2Regularization"> The L2 regularization parameter. </param>
        AsgdOptimizer(uint64_t dim, LossFunctionType lossFunction, double l2Regularization);

        /// <summary> Performs a given number of learning iterations. </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of example iterator to use. </typeparam>
        /// <param name="exampleIterator"> [in,out] The data iterator. </param>
        /// <param name="numExamples"> The number of examples in the iterator. </param>
        template<typename ExampleIteratorType>
        void Update(ExampleIteratorType& exampleIterator);

        /// <summary> Performs a given number of learning iterations. </summary>
        ///
        /// <param name="exampleIterator"> [in,out] The data iterator. </param>
        /// <param name="numExamples"> The number of examples in the iterator. </param>
        void Update(utilities::AnyIterator<dataset::SupervisedExample>& exampleIterator);

        /// <summary> Returns The averaged predictor. </summary>
        ///
        /// <returns> The averaged predictor. </returns>
        const predictors::LinearPredictor& GetPredictor() const;
            
    private:
        LossFunctionType _lossFunction;
        double _lambda = 0;

        uint64_t _total_iterations = 0;
        predictors::LinearPredictor _lastPredictor;
        predictors::LinearPredictor _averagedPredictor;
    };
}

#include "../tcc/AsgdOptimizer.tcc"
