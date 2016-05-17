////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"
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
    struct MultiEpochTrainerParameters
    {
        uint64_t epochSize = 0;
        uint64_t numEpochs = 1;
        std::string dataPermutationRandomSeed = 0;
    };

    /// <summary> A class that performs multiple epochs of an IncrementalTrainer and exposes a trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class MultiEpochTrainer : public ITrainer<PredictorType>
    {
    public:
        MultiEpochTrainer() = delete;

        /// <summary> Constructs an instance of MultiEpochTrainer. </summary>
        ///
        /// <param name="incrementalTrainer"> [in,out] The stateful trainer. </param>
        MultiEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer, const MultiEpochTrainerParameters& parameters);

        /// <summary> Trains and returns a predictor. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        ///
        /// <returns> The trained predictor. </returns>
        virtual PredictorType Train(dataset::GenericRowDataset::Iterator exampleIterator) const;

    private:
        std::unique_ptr<IIncrementalTrainer<PredictorType>> _incrementalTrainer;
        MultiEpochTrainerParameters _parameters;
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
    std::unique_ptr<ITrainer<PredictorType>> MakeMultiEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer, const MultiEpochTrainerParameters& parameters);
}

#include "../tcc/MultiEpochTrainer.tcc"
