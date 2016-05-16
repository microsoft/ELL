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

namespace trainers
{
    /// <summary> Parameters for the multi-epoch meta-trainer. </summary>
    struct MultiEpochTrainerParameters
    {
        uint64_t numEpochs = 1;
    };

    /// <summary> A class that performs multiple epochs of a learner and exposes a trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class MultiEpochTrainer : public ITrainer<PredictorType>
    {
    public:
        MultiEpochTrainer() = delete;

        /// <summary> Constructs an instance of MultiEpochTrainer. </summary>
        ///
        /// <param name="learner"> [in,out] The stateful trainer. </param>
        MultiEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& learner, const MultiEpochTrainerParameters& parameters);

        /// <summary> Trains and returns a predictor. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        ///
        /// <returns> The trained predictor. </returns>
        virtual PredictorType Train(dataset::GenericRowDataset::Iterator exampleIterator) const;

    private:
        std::unique_ptr<IIncrementalTrainer<PredictorType>> _learner;
        MultiEpochTrainerParameters _parameters;
    };

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMultiEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& learner);
}

#include "../tcc/MultiEpochTrainer.tcc"
