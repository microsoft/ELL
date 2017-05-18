////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MakeTrainer.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ForestTrainerArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"
#include "TrainerArguments.h"

// predictors
#include "LinearPredictor.h"

// trainers
#include "ITrainer.h"
#include "SGDLinearTrainer.h"

// stl
#include <memory>

namespace ell
{
namespace common
{
    /// <summary> Makes a stochastic gradient descent trainer. </summary>
    ///
    /// <param name="lossArguments"> loss arguments. </param>
    /// <param name="trainerArguments"> trainer arguments. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSGDLinearTrainer(const LossArguments& lossArguments, const trainers::SGDLinearTrainerParameters& trainerParameters);

    /// <summary> Makes a stochastic gradient descent trainer for sparse data. </summary>
    ///
    /// <param name="lossArguments"> loss arguments. </param>
    /// <param name="trainerArguments"> trainer arguments. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSDSGDLinearTrainer(const LossArguments& lossArguments, const trainers::SGDLinearTrainerParameters& trainerParameters);

    /// <summary> Makes a stochastic gradient descent trainer for centered sparse data. </summary>
    ///
    /// <param name="lossArguments"> loss arguments. </param>
    /// <param name="center"> The center (mean) of the training set. </param>
    /// <param name="trainerArguments"> trainer arguments. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSDCSGDLinearTrainer(const LossArguments& lossArguments, math::RowVector<double> center, const trainers::SGDLinearTrainerParameters& trainerParameters);

    /// <summary> Makes a forest trainer. </summary>
    ///
    /// <param name="lossArguments"> loss arguments. </param>
    /// <param name="trainerArguments"> trainer arguments. </param>
    ///
    /// <returns> A unique_ptr to a forest trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::SimpleForestPredictor>> MakeForestTrainer(const LossArguments& lossArguments, const ForestTrainerArguments& trainerArguments);
}
}
