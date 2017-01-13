////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MakeTrainer.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MakeTrainer.h"

// lossFunctions
#include "HingeLoss.h"
#include "LogLoss.h"
#include "SquaredLoss.h"

// utilities
#include "CommandLineParser.h"

// trainers
#include "HistogramForestTrainer.h"
#include "LogitBooster.h"
#include "SortingForestTrainer.h"
#include "ThresholdFinder.h"

namespace ell
{
namespace common
{
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeStochasticGradientDescentTrainer(size_t dim, const LossArguments& lossArguments, const StochasticGradientDescentTrainerArguments& trainerArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
            case LossFunctionEnum::squared:
                return trainers::MakeStochasticGradientDescentTrainer(dim, lossFunctions::SquaredLoss(), trainerArguments);

            case LossFunctionEnum::log:
                return trainers::MakeStochasticGradientDescentTrainer(dim, lossFunctions::LogLoss(lossArguments.lossFunctionParameter), trainerArguments);

            case LossFunctionEnum::hinge:
                return trainers::MakeStochasticGradientDescentTrainer(dim, lossFunctions::HingeLoss(), trainerArguments);

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::SimpleForestPredictor>> MakeForestTrainer(const LossArguments& lossArguments, const ForestTrainerArguments& trainerArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
            case LossFunctionEnum::squared:

                if (trainerArguments.sortingTrainer)
                {
                    return trainers::MakeSortingForestTrainer(lossFunctions::SquaredLoss(), trainers::LogitBooster(), trainerArguments);
                }
                else
                {
                    return trainers::MakeHistogramForestTrainer(lossFunctions::SquaredLoss(), trainers::LogitBooster(), trainers::ExhaustiveThresholdFinder(), trainerArguments);
                }

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }
}
}
