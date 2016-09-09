////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SweepingIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EvaluatingIncrementalTrainer.h"
#include "IIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainer.h"

// dataset
#include "RowDataset.h"

// evaluators
#include "Evaluator.h"

//stl
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace emll
{
namespace trainers
{
    /// <summary> A class that runs multiple internal trainers and chooses the best performing predictor. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class SweepingIncrementalTrainer : public IIncrementalTrainer<PredictorType>
    {
    public:
        typedef EvaluatingIncrementalTrainer<PredictorType> EvaluatingTrainerType;

        /// <summary> Constructs an instance of SweepingIncrementalTrainer. </summary>
        ///
        /// <param name="evaluatingTrainers"> A vector of evaluating trainers. </param>
        /// <param name="parameters"> Multi-epoch training parameter. </param>
        SweepingIncrementalTrainer(std::vector<EvaluatingTrainerType>&& evaluatingTrainers, const MultiEpochIncrementalTrainerParameters& parameters);

        /// <summary> Perform a sequence of training epochs. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const PredictorType> GetPredictor() const override;

    private:
        std::vector<EvaluatingTrainerType> _evaluatingTrainers;
        MultiEpochIncrementalTrainerParameters _parameters;
        mutable std::default_random_engine _random;
    };

    /// <summary> Makes an incremental trainer that runs multiple internal trainers and chooses the best performing predictor. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="evaluatingTrainers"> A vector of evaluating trainers. </param>
    ///
    /// <returns> A unique_ptr to a sweeping trainer. </returns>
    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeSweepingIncrementalTrainer(std::vector<EvaluatingIncrementalTrainer<PredictorType>>&& evaluatingTrainers, const MultiEpochIncrementalTrainerParameters& parameters);
}
}

#include "../tcc/SweepingIncrementalTrainer.tcc"
