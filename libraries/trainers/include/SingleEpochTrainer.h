////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleEpochTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"
#include "ILearner.h"

// dataset
#include "RowDataset.h"

//stl
#include <utility>
#include <memory>

namespace trainers
{
    /// <summary> A class that wraps a learner and exposes a trainer. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class SingleEpochTrainer : public ITrainer<PredictorType>
    {
    public:
        SingleEpochTrainer() = delete;

        /// <summary> Constructs an instance of SingleEpochTrainer. </summary>
        ///
        /// <param name="learner"> [in,out] The stateful trainer. </param>
        SingleEpochTrainer(std::unique_ptr<ILearner<PredictorType>>&& learner);
        
        /// <summary> Trains and returns a predictor. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        ///
        /// <returns> The trained predictor. </returns>
        virtual PredictorType Train(dataset::GenericRowDataset::Iterator exampleIterator) const;

    private: 
        std::unique_ptr<ILearner<PredictorType>> _learner;
    };

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeSingleEpochTrainer(std::unique_ptr<ILearner<PredictorType>>&& learner);
}

#include "../tcc/SingleEpochTrainer.tcc"
