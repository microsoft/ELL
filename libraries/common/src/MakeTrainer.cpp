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
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSGDLinearTrainer(const LossArguments& lossArguments, const trainers::SGDLinearTrainerParameters& trainerParameters)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSGDLinearTrainer(lossFunctions::SquaredLoss(), trainerParameters);

        case LossFunctionEnum::log:
            return trainers::MakeSGDLinearTrainer(lossFunctions::LogLoss(lossArguments.lossFunctionParameter), trainerParameters);

        case LossFunctionEnum::hinge:
            return trainers::MakeSGDLinearTrainer(lossFunctions::HingeLoss(), trainerParameters);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSDSGDLinearTrainer(const LossArguments& lossArguments, const trainers::SDSGDLinearTrainerParameters& trainerParameters)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSDSGDLinearTrainer(lossFunctions::SquaredLoss(), trainerParameters);

        case LossFunctionEnum::log:
            return trainers::MakeSDSGDLinearTrainer(lossFunctions::LogLoss(lossArguments.lossFunctionParameter), trainerParameters);

        case LossFunctionEnum::hinge:
            return trainers::MakeSDSGDLinearTrainer(lossFunctions::HingeLoss(), trainerParameters);

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
