////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleEpochTrainer.h (trainers)
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
    /// <summary> A class that wraps an IncrementalTrainer and exposes a trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class SingleEpochTrainer : public ITrainer<PredictorType>
    {
    public:
        SingleEpochTrainer() = delete;

        /// <summary> Constructs an instance of SingleEpochTrainer. </summary>
        ///
        /// <param name="incrementalTrainer"> [in,out] The stateful trainer. </param>
        SingleEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer);
        
        /// <summary> Trains and returns a predictor. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        ///
        /// <returns> The trained predictor. </returns>
        virtual PredictorType Train(dataset::GenericRowDataset::Iterator exampleIterator) const;

    private: 
        std::unique_ptr<IIncrementalTrainer<PredictorType>> _incrementalTrainer;
    };

    /// <summary> Makes a trainer that runs an incremental trainer for a single epoch. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="incrementalTrainer"> [in,out] The incremental trainer. </param>
    ///
    /// <returns> A unique_ptr to a single epoch trainer. </returns>
    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeSingleEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer);
}

#include "../tcc/SingleEpochTrainer.tcc"
