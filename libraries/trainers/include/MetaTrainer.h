////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MetaTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"
#include "IStatefulTrainer.h"

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
    class MetaTrainer : public ITrainer<PredictorType>
    {
    public:
        MetaTrainer() = delete;

        /// <summary> Constructs an instance of MetaTrainer. </summary>
        ///
        /// <param name="statefulTrainer"> [in,out] The stateful trainer. </param>
        MetaTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer);
        
        /// <summary> Trains and returns a predictor. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        ///
        /// <returns> The trained predictor. </returns>
        virtual PredictorType Train(dataset::GenericRowDataset::Iterator exampleIterator) const;

    private: 
        std::unique_ptr<IStatefulTrainer<PredictorType>> _statefulTrainer;
    };

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMetaTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer);
}

#include "../tcc/MetaTrainer.tcc"
