////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MakeTrainer.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ForestTrainerArguments.h"
#include "TrainerArguments.h"
#include "ProtoNNTrainerArguments.h"

// predictors
#include "LinearPredictor.h"
#include "ProtoNNPredictor.h"

// trainers
#include "ITrainer.h"
#include "SGDTrainer.h"
#include "SDCATrainer.h"

// stl
#include <memory>

namespace ell
{
namespace common
{
    /// <summary> Makes a stochastic gradient descent trainer. </summary>
    ///
    /// <param name="lossFunctionArguments"> loss arguments. </param>
    /// <param name="trainerParameters"> trainer parameters. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSGDTrainer(const LossFunctionArguments& lossFunctionArguments, const trainers::SGDTrainerParameters& trainerParameters);

    /// <summary> Makes a stochastic gradient descent trainer for sparse data. </summary>
    ///
    /// <param name="lossFunctionArguments"> loss arguments. </param>
    /// <param name="trainerParameters"> trainer parameters. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSparseDataSGDTrainer(const LossFunctionArguments& lossFunctionArguments, const trainers::SGDTrainerParameters& trainerParameters);

    /// <summary> Makes a stochastic gradient descent trainer for centered sparse data. </summary>
    ///
    /// <param name="lossFunctionArguments"> loss arguments. </param>
    /// <param name="center"> The center (mean) of the training set. </param>
    /// <param name="trainerParameters"> trainer parameters. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSparseDataCenteredSGDTrainer(const LossFunctionArguments& lossFunctionArguments, math::RowVector<double> center, const trainers::SGDTrainerParameters& trainerParameters);

    /// <summary> Makes a stochastic dual coordinate ascent trainer. </summary>
    ///
    /// <param name="lossFunctionArguments"> loss arguments. </param>
    /// <param name="trainerParameters"> trainer parameters. </param>
    ///
    /// <returns> A unique_ptr to a stochastic dual coordinate ascent trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSDCATrainer(const LossFunctionArguments& lossFunctionArguments, const trainers::SDCATrainerParameters& trainerParameters);

    /// <summary> Makes a forest trainer. </summary>
    ///
    /// <param name="lossFunctionArguments"> loss arguments. </param>
    /// <param name="trainerArguments"> trainer arguments. </param>
    ///
    /// <returns> A unique_ptr to a forest trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::SimpleForestPredictor>> MakeForestTrainer(const LossFunctionArguments& lossFunctionArguments, const ForestTrainerArguments& trainerArguments);

    /// <summary> Makes a protoNN trainer. </summary>
    ///
    /// <param name="parameters"> trainer arguments. </param>
    ///
    /// <returns> A unique_ptr to a protoNN trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::ProtoNNPredictor>> MakeProtoNNTrainer(const trainers::ProtoNNTrainerParameters& parameters);
}
}
