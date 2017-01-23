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
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeLinearSGDTrainer(const LossArguments& lossArguments, const trainers::LinearSGDTrainerParameters& trainerParameters)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeLinearSGDTrainer(lossFunctions::SquaredLoss(), trainerParameters);

        case LossFunctionEnum::log:
            return trainers::MakeLinearSGDTrainer(lossFunctions::LogLoss(lossArguments.lossFunctionParameter), trainerParameters);

        case LossFunctionEnum::hinge:
            return trainers::MakeLinearSGDTrainer(lossFunctions::HingeLoss(), trainerParameters);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeLinearSparseDataSGDTrainer(const LossArguments& lossArguments, const trainers::LinearSparseDataSGDTrainerParameters& trainerParameters)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeLinearSparseDataSGDTrainer(lossFunctions::SquaredLoss(), trainerParameters);

        case LossFunctionEnum::log:
            return trainers::MakeLinearSparseDataSGDTrainer(lossFunctions::LogLoss(lossArguments.lossFunctionParameter), trainerParameters);

        case LossFunctionEnum::hinge:
            return trainers::MakeLinearSparseDataSGDTrainer(lossFunctions::HingeLoss(), trainerParameters);

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
