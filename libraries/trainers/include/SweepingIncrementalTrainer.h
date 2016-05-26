////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SweepingIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainer.h"

// dataset
#include "RowDataset.h"

// evaluators
#include "Evaluator.h"

//stl
#include <utility>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace trainers
{
    /// <summary> A class that performs multiple epochs of an IncrementalTrainer. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class SweepingIncrementalTrainer : public IIncrementalTrainer<PredictorType>
    {
    public:
        SweepingIncrementalTrainer() = delete;

        /// <summary> Constructs an instance of SweepingIncrementalTrainer. </summary>
        ///
        /// <param name="incrementalTrainers"> A vector of incremental trainers. </param>
        /// <param name="parameters"> Multi-epoch training parameters. </param>
        /// <param name="evaluator"> A vector of evaluators. </param>
        SweepingIncrementalTrainer(
            std::vector<std::unique_ptr<IIncrementalTrainer<PredictorType>>>&& incrementalTrainers, 
            const MultiEpochIncrementalTrainerParameters& parameters, 
            std::vector<std::shared_ptr<evaluators::IEvaluator<PredictorType>>> evaluators);

        /// <summary> Perform a set of training epochs. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const PredictorType> GetPredictor() const override;

    private:
        std::vector<std::unique_ptr<IIncrementalTrainer<PredictorType>>> _incrementalTrainers;
        MultiEpochIncrementalTrainerParameters _parameters;
        std::vector<std::shared_ptr<evaluators::IEvaluator<PredictorType>>> _evaluators;
        mutable std::default_random_engine _random;
    };

    /// <summary> Makes an incremental trainer that runs multiple internal incremental trainers in parallel and uses an evaluator to choose the best one. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="incrementalTrainers"> A vector of incremental trainers. </param>
    /// <param name="parameters"> Multi-epoch training parameters. </param>
    /// <param name="evaluator"> A vector of evaluators. </param>
    ///
    /// <returns> A unique_ptr to a sweeping trainer. </returns>
    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeSweepingIncrementalTrainer(
        std::vector<std::unique_ptr<IIncrementalTrainer<PredictorType>>>&& incrementalTrainers,
        const MultiEpochIncrementalTrainerParameters& parameters,
        std::vector<std::shared_ptr<evaluators::IEvaluator<PredictorType>>> evaluator);
}

#include "../tcc/SweepingIncrementalTrainer.tcc"
