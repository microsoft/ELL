////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StochasticGradientDescentLearner.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IStatefulTrainer.h"

// predictors
#include "LinearPredictor.h"

// linear
#include "DoubleVector.h"

// dataset
#include "SupervisedExample.h"
#include "RowDataset.h"

// stl
#include <cstdint>
#include <memory>

namespace trainers
{
    /// <summary> Parameters for the stochastic gradient descent trainer. </summary>
    struct StochasticGradientDescentLearnerParameters
    {
        double regularization = 1.0;
    };
    
    /// <summary>
    /// Implements the averaged stochastic gradient descent algorithm on an L2 regularized empirical
    /// loss.
    /// </summary>
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    template <typename LossFunctionType>
    class StochasticGradientDescentLearner : public IStatefulTrainer<predictors::LinearPredictor>
    {
    public:

        /// <summary> Constructs the trainer. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        /// <param name="parameters"> The training Parameters. </param>
        /// <param name="lossFunction"> The loss function. </param>
        StochasticGradientDescentLearner(uint64_t dim, const StochasticGradientDescentLearnerParameters& parameters, const LossFunctionType& lossFunction);

        /// <summary> Performs an epoch of SGD iterations. </summary>
        ///
        /// <param name="exampleIterator"> [in,out] The data iterator. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Returns The averaged predictor. </summary>
        ///
        /// <returns> The averaged predictor. </returns>
        virtual const predictors::LinearPredictor& GetPredictor() const override;

        /// <summary> Resets the trainer and returns its current predictor. </summary>
        ///
        /// <returns> The current predictor. </returns>
        virtual predictors::LinearPredictor Reset() override;

    private:
        StochasticGradientDescentLearnerParameters _parameters;
        LossFunctionType _lossFunction;

        uint64_t _total_iterations = 0;
        predictors::LinearPredictor _lastPredictor;
        predictors::LinearPredictor _averagedPredictor;
    };

    /// <summary> Makes a sorting tree trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="dim"> The dimension. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    /// <param name="lossFunction"> The loss function. </param>
    ///
    /// <returns> A sorting tree trainer </returns>
    template <typename LossFunctionType>
    std::unique_ptr<IStatefulTrainer<predictors::LinearPredictor>> MakeStochasticGradientDescentLearner(uint64_t dim, const StochasticGradientDescentLearnerParameters& parameters, const LossFunctionType& lossFunction);
}

#include "../tcc/StochasticGradientDescentLearner.tcc"
