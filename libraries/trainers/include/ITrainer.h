////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ITrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// data
#include "Dataset.h"

// stl
#include <memory>

namespace ell
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
    class ITrainer
    {
    public:
        virtual ~ITrainer() = default;

        /// <summary> Sets the trainer's dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        virtual void SetDataset(const data::AnyDataset& anyDataset) = 0;

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        virtual void Update() = 0;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A const reference to the current predictor. </returns>
        virtual const PredictorType& GetPredictor() const = 0;
    };
}
}
