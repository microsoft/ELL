////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultiEpochIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"

// data
#include "Dataset.h"

// evaluators
#include "Evaluator.h"

//stl
#include <memory>
#include <random>
#include <string>
#include <utility>

namespace ell
{
namespace trainers
{
    /// <summary> Parameters for the multi-epoch meta-trainer. </summary>
    struct MultiEpochIncrementalTrainerParameters
    {
        size_t epochSize = 0;
        size_t numEpochs = 1;
        std::string dataPermutationRandomSeed = "";
    };

    /// <summary> A class that performs multiple epochs of another incremental trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class MultiEpochIncrementalTrainer : public ITrainer<PredictorType>
    {
    public:
        typedef ITrainer<PredictorType> InternalTrainerType;

        /// <summary> Constructs an instance of MultiEpochIncrementalTrainer. </summary>
        ///
        /// <param name="internalTrainer"> An incremental trainer. </param>
        /// <param name="parameters"> Multi-epoch training parameters. </param>
        MultiEpochIncrementalTrainer(std::unique_ptr<InternalTrainerType>&& internalTrainer, const MultiEpochIncrementalTrainerParameters& parameters);

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        virtual void Update(const data::AnyDataset& anyDataset) override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A const reference to the current predictor. </returns>
        virtual const PredictorType& GetPredictor() const override { return _internalTrainer->GetPredictor(); }

    private:
        std::unique_ptr<InternalTrainerType> _internalTrainer;
        MultiEpochIncrementalTrainerParameters _parameters;
        mutable std::default_random_engine _random;
    };

    /// <summary> Makes an incremental trainer that runs another incremental trainer for multiple epochs. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="internalTrainer"> The internal trainer. </param>
    /// <param name="parameters"> Multi-epoch training parameters. </param>
    ///
    /// <returns> A unique_ptr to a multi-epoch trainer. </returns>
    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMultiEpochIncrementalTrainer(
        std::unique_ptr<ITrainer<PredictorType>>&& internalTrainer,
        const MultiEpochIncrementalTrainerParameters& parameters);
}
}

#include "../tcc/MultiEpochIncrementalTrainer.tcc"
