////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MakeTrainer.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TrainerArguments.h"

// predictors
#include "LinearPredictor.h"

// trainers
#include "ITrainer.h"
#include "ILearner.h"
#include "SortingTreeTrainer.h"
#include "StochasticGradientDescentLearner.h"

// stl
#include <memory>

namespace common
{
    /// <summary> Makes a stochastic gradient descent trainer. </summary>
    ///
    /// <param name="parameters"> Trainer parameters. </param>
    /// <param name="trainerArguments"> Trainer command line arguments. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::ILearner<predictors::LinearPredictor>> MakeStochasticGradientDescentLearner(uint64_t dim, const trainers::StochasticGradientDescentLearnerParameters& parameters, const LossArguments& lossArguments);

    /// <summary> Makes a sorting tree trainer. </summary>
    ///
    /// <param name="parameters"> Trainer parameters. </param>
    /// <param name="trainerArguments"> Trainer command line arguments. </param>
    ///
    /// <returns> A unique_ptr to a sorting tree trainer. </returns>
    std::unique_ptr<trainers::ITrainer<predictors::DecisionTreePredictor>> MakeSortingTreeTrainer(const trainers::SortingTreeTrainerParameters& parameters, const LossArguments& lossArguments);
}