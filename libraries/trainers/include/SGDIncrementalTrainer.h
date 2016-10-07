////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SGDIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainer.h"

// predictors
#include "LinearPredictor.h"

// linear
#include "DoubleVector.h"

// dataset
#include "Example.h"
#include "RowDataset.h"

// stl
#include <cstdint>
#include <memory>

namespace emll
{
namespace trainers
{
    /// <summary> Parameters for the stochastic gradient descent trainer. </summary>
    struct SGDIncrementalTrainerParameters
    {
        double regularization;
    };

    /// <summary>
    /// Implements the averaged stochastic gradient descent algorithm on an L2 regularized empirical
    /// loss.
    /// </summary>
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    template <typename LossFunctionType>
    class SGDIncrementalTrainer : public IIncrementalTrainer<predictors::LinearPredictor>
    {
    public:
        typedef predictors::LinearPredictor PredictorType;

        /// <summary> Constructs the trainer. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="parameters"> The training parameters. </param>
        SGDIncrementalTrainer(uint64_t dim, const LossFunctionType& lossFunction, const SGDIncrementalTrainerParameters& parameters);

        /// <summary> Performs an epoch of SGD iterations. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::ExampleIterator<dataset::AutoSupervisedExample> exampleIterator) override;

        void Update(dataset::DataSet dataSet)  // TODO make this the real thing!
        {
            auto exampleIterator = dataSet.GetIterator<dataset::AutoSupervisedExample>();
            Update(exampleIterator);
        }

        /// <summary> Returns The averaged predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const PredictorType> GetPredictor() const override { return _averagedPredictor; }

    private:
        void UpdateSparse(dataset::ExampleIterator<dataset::AutoSupervisedExample> exampleIterator);
        void UpdateDense(dataset::ExampleIterator<dataset::AutoSupervisedExample> exampleIterator);

        LossFunctionType _lossFunction;
        SGDIncrementalTrainerParameters _parameters;

        uint64_t _total_iterations = 0;
        PredictorType _lastPredictor;
        std::shared_ptr<PredictorType> _averagedPredictor;
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
    std::unique_ptr<trainers::IIncrementalTrainer<predictors::LinearPredictor>> MakeSGDIncrementalTrainer(uint64_t dim, const LossFunctionType& lossFunction, const SGDIncrementalTrainerParameters& parameters);
}
}

#include "../tcc/SGDIncrementalTrainer.tcc"
