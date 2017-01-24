////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDLinearTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"
#include "MultiEpochIncrementalTrainer.h"

// predictors
#include "LinearPredictor.h"

// data
#include "Dataset.h"
#include "Example.h"

// stl
#include <cstddef>
#include <memory>

namespace ell
{
namespace trainers
{
    /// <summary> Parameters for the stochastic gradient descent trainer. </summary>
    struct SGDLinearTrainerParameters
    {
        double regularization;
    };

    /// <summary>
    /// Implements the averaged stochastic gradient descent algorithm on an L2 regularized empirical
    /// loss.
    /// </summary>
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    template <typename LossFunctionType>
    class SGDLinearTrainer : public ITrainer<predictors::LinearPredictor>
    {
    public:
        typedef predictors::LinearPredictor PredictorType;

        /// <summary> Constructs the trainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="parameters"> The training parameters. </param>
        SGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters);

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        virtual void Update(const data::AnyDataset& anyDataset) override;

        /// <summary> Returns The averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        virtual const PredictorType& GetPredictor() const override { return _averagedPredictor; }

        /// <summary> Returns a const reference to the last predictor. </summary>
        ///
        /// <returns> A const reference to the last predictor. </returns>
        const PredictorType& GetLastPredictor() const { return _lastPredictor; }

        /// <summary> Returns a const reference to the averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        const PredictorType& GetAveragedPredictor() const { return *_averagedPredictor; }

    private:
        LossFunctionType _lossFunction;
        SGDLinearTrainerParameters _parameters;

        double _t = 0;
        PredictorType _lastPredictor;
        PredictorType _averagedPredictor;
    };

    /// <summary> Makes a sorting tree trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="parameters"> The trainer parameters. </param>
    /// <param name="lossFunction"> The loss function. </param>
    ///
    /// <returns> A sorting tree trainer </returns>
    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters);
}
}

#include "../tcc/SGDLinearTrainer.tcc"
