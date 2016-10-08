////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IIncrementalTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// data
#include "RowDataset.h"

// stl
#include <memory>

namespace emll
{
namespace trainers
{
    /// <summary>
    /// Interface to a stateful training algorithm that keeps a predictor as part of its state and
    /// can perform an arbitray number of updates.
    /// </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor created by the trainer. </typeparam>
    template <typename PredictorType>
    class IIncrementalTrainer
    {
    public:
        virtual ~IIncrementalTrainer() = default;

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        ///
        /// <param name="dataset"> A dataset. </param>
        virtual void Update(data::Dataset dataset) = 0;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const std::shared_ptr<const PredictorType> GetPredictor() const = 0;
    };
}
}
