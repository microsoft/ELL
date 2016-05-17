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
        std::string dataPermutationRandomSeed = 0;
    };

    /// <summary> A class that performs multiple epochs of an IncrementalTrainer and exposes a trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class MultiEpochIncrementalTrainer : public IIncrementalTrainer<PredictorType>
    {
    public:
        MultiEpochIncrementalTrainer() = delete;

        /// <summary> Constructs an instance of MultiEpochIncrementalTrainer. </summary>
        ///
        /// <param name="incrementalTrainer"> [in,out] The stateful trainer. </param>
        MultiEpochIncrementalTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer, const MultiEpochIncrementalTrainerParameters& parameters);

        /// <summary> Perform a set of training epochs. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) override;

        /// <summary> Returns the trained predictor and resets the trainer to its initial state. </summary>
        ///
        /// <returns> The current trained predictor. </returns>
        virtual PredictorType Reset() override { return _incrementalTrainer->Reset(); }

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A constant reference to the current predictor. </returns>
        virtual const PredictorType& GetPredictor() const override { return _incrementalTrainer->GetPredictor(); }

    private:
        std::unique_ptr<IIncrementalTrainer<PredictorType>> _incrementalTrainer;
        MultiEpochIncrementalTrainerParameters _parameters;
        mutable std::default_random_engine _random;
    };

    /// <summary> Makes a trainer that runs an incremental trainer for multiple epochs. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="incrementalTrainer"> [in,out] The incremental trainer. </param>
    /// <param name="parameters"> Parameters for the multi-epoch trainer. </param>
    ///
    /// <returns> A unique_ptr to a multi-epoch trainer. </returns>
    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeMultiEpochIncrementalTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer, const MultiEpochIncrementalTrainerParameters& parameters);
}

#include "../tcc/MultiEpochIncrementalTrainer.tcc"
