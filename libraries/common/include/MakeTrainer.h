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
#include "SortingTreeTrainerArguments.h"

// predictors
#include "LinearPredictor.h"

// trainers
#include "ITrainer.h"
#include "IIncrementalTrainer.h"
#include "SortingTreeTrainer.h"
#include "SGDIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainer.h"

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

    /// <summary> Makes a multi-epoch stochastic gradient descent incremental trainer. </summary>
    ///
    /// <param name="dim"> The dimension. </param>
    /// <param name="lossArguments"> The loss arguments. </param>
    /// <param name="sgdArguments"> The sgd arguments. </param>
    /// <param name="multiEpochArguments"> The multi epoch arguments. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::IIncrementalTrainer<predictors::LinearPredictor>> MakeMultiEpochSGDIncrementalTrainer(uint64_t dim, const LossArguments& lossArguments, const SGDIncrementalTrainerArguments& sgdArguments, const MultiEpochIncrementalTrainerArguments& multiEpochArguments);

    /// <summary> Makes a sorting tree trainer. </summary>
    ///
    /// <param name="parameters"> Trainer parameters. </param>
    /// <param name="trainerArguments"> Trainer command line arguments. </param>
    ///
    /// <returns> A unique_ptr to a sorting tree trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::DecisionTreePredictor>> MakeSortingTreeTrainer(const LossArguments& lossArguments, const SortingTreeTrainerArguments& sortingTreeArguments);



}