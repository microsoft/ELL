////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MakeTrainer.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TrainerArguments.h"
#include "SGDIncrementalTrainerArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"
#include "ForestTrainerArguments.h"

// predictors
#include "LinearPredictor.h"

// trainers
#include "IIncrementalTrainer.h"

// stl
#include <memory>

namespace common
{
    /// <summary> Makes a stochastic gradient descent incremental trainer. </summary>
    ///
    /// <param name="dim"> The dimension. </param>
    /// <param name="lossArguments"> The loss arguments. </param>
    /// <param name="parameters"> SGD parameters. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::IIncrementalTrainer<predictors::LinearPredictor>> MakeSGDIncrementalTrainer(uint64_t dim, const LossArguments& lossArguments, const SGDIncrementalTrainerArguments& sgdArguments);

    /// <summary> Makes a sorting forest trainerr. </summary>
    ///
    /// <param name="parameters"> Trainer parameters. </param>
    /// <param name="trainerArguments"> Trainer command line arguments. </param>
    ///
    /// <returns> A unique_ptr to a forest trainer. </returns>
    std::unique_ptr<trainers::IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeSortingForestTrainer(const LossArguments& lossArguments, const ForestTrainerArguments& sortingTreeArguments);

    /// <summary> Makes a simple forest trainerr. </summary>
    ///
    /// <param name="parameters"> Trainer parameters. </param>
    /// <param name="trainerArguments"> Trainer command line arguments. </param>
    ///
    /// <returns> A unique_ptr to a forest trainer. </returns>
    std::unique_ptr<trainers::IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeHistogramForestTrainer(const LossArguments& lossArguments, const ForestTrainerArguments& sortingTreeArguments);
}