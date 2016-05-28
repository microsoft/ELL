////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIncrementalTrainer.h"

// dataset
#include "RowDataset.h"

// evaluators
#include "Evaluator.h"

//stl
#include <utility>
#include <memory>
#include <random>
#include <string>

namespace trainers
{
    /// <summary> Parameters for the multi-epoch meta-trainer. </summary>
    struct MultiEpochIncrementalTrainerParameters
    {
        uint64_t epochSize = 0;
        uint64_t numEpochs = 1;
        std::string dataPermutationRandomSeed = "";
    };

    /// <summary> A class that performs multiple epochs of another incremental trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class MultiEpochIncrementalTrainer : public IIncrementalTrainer<PredictorType>
    {
    public:
        typedef IIncrementalTrainer<PredictorType> InternalTrainerType;

        /// <summary> Constructs an instance of MultiEpochIncrementalTrainer. </summary>
        ///
        /// <param name="internalTrainer"> An incremental trainer. </param>
        /// <param name="parameters"> Multi-epoch training parameters. </param>
        MultiEpochIncrementalTrainer(
            std::unique_ptr<InternalTrainerType>&& internalTrainer, 
            const MultiEpochIncrementalTrainerParameters& parameters);

        /// <summary> Perform a set of training epochs. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const PredictorType> GetPredictor() const override { return _internalTrainer->GetPredictor(); }

    private:
        std::unique_ptr<InternalTrainerType> _internalTrainer;
        MultiEpochIncrementalTrainerParameters _parameters;
        mutable std::default_random_engine _random;
    };

    /// <summary> Makes an incremental trainer that runs another incremental trainer for multiple epochs. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="incrementalTrainer"> An incremental trainer. </param>
    /// <param name="parameters"> Multi-epoch training parameters. </param>
    ///
    /// <returns> A unique_ptr to a multi-epoch trainer. </returns>
    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeMultiEpochIncrementalTrainer(
        std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer, 
        const MultiEpochIncrementalTrainerParameters& parameters);
}

#include "../tcc/MultiEpochIncrementalTrainer.tcc"
