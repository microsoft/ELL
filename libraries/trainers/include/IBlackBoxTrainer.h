////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IBlackBoxTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"

namespace trainers
{
    /// <summary> Interface to a black box (stateless) training algorithm that takes a data iterator and returns a predictor. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor created by the trainer. </typeparam>
    template <typename PredictorType>
    class IBlackBoxTrainer
    {
    public:

        virtual ~IBlackBoxTrainer() = default;

        /// <summary> Trains and returns a predictor. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the training set. </param>
        ///
        /// <returns> The trained predictor. </returns>
        virtual PredictorType Train(dataset::GenericRowDataset::Iterator exampleIterator) const = 0;
    };
}
