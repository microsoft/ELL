// AsgdOptimizer.h

#pragma once

// predictors
#include "SharedLinearBinaryPredictor.h"
using predictors::SharedLinearBinaryPredictor;

// linear
#include "DoubleVector.h"
using linear::DoubleVector;

#include "types.h"


namespace convex_optimization
{
    /// Implements the Averaged Stochstic Gradient Descent algorithm on an L2 regularized empirical loss
    ///
    class AsgdOptimizer
    {
    public:

        /// Constructs the optimizer
        ///
        AsgdOptimizer(uint64 dim);

        /// Performs a given number of learning iterations
        ///
        template<typename ExampleIteratorType, typename LossFunctionType>
        void Update(ExampleIteratorType& data_iter, const LossFunctionType& loss_function, double l2Regularization);

        /// \returns The averaged predictor
        ///
        const SharedLinearBinaryPredictor GetPredictor() const;
            
    private:
        uint64 _total_iterations;
        DoubleVector _w;
        double _b;
        SharedLinearBinaryPredictor _predictor;
    };
}

#include "../tcc/AsgdOptimizer.tcc"
