// AsgdOptimizer.h

#pragma once

#include "types.h"

// predictors
#include "SharedLinearBinaryPredictor.h"

// linear
#include "DoubleVector.h"

namespace optimization
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
        const predictors::SharedLinearBinaryPredictor GetPredictor() const;
            
    private:
        uint64 _total_iterations;
        linear::DoubleVector _w;
        double _b;
        predictors::SharedLinearBinaryPredictor _predictor;
    };
}

#include "../tcc/AsgdOptimizer.tcc"
