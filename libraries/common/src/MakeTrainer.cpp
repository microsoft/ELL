////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MakeTrainer.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MakeTrainer.h"

// lossFunctions
#include "SquaredLoss.h"
#include "LogLoss.h"
#include "HingeLoss.h"

// utilities
#include "CommandLineParser.h"

namespace common
{
    std::unique_ptr<trainers::IIncrementalTrainer<predictors::LinearPredictor>> MakeSGDIncrementalTrainer(uint64_t dim, const LossArguments& lossArguments, const trainers::SGDIncrementalTrainerParameters& parameters)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::SquaredLoss(), parameters);

        case LossFunctionEnum::log:
            return trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::LogLoss(lossArguments.lossFunctionParameter), parameters);

        case LossFunctionEnum::hinge:
            return trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::HingeLoss(), parameters);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::DecisionTreePredictor>> MakeSortingTreeTrainer(const LossArguments& lossArguments, const trainers::SortingTreeTrainerParameters& parameters)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSortingTreeTrainer(lossFunctions::SquaredLoss(), parameters);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSGDTrainer(uint64_t dim, const LossArguments& lossArguments, const SGDIncrementalTrainerArguments& sgdArguments, const MultiEpochTrainerArguments& multiEpochArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
            case LossFunctionEnum::squared:
            {
                auto sgdIncrementalTrainer = trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::SquaredLoss(), sgdArguments);
                return trainers::MakeMultiEpochTrainer(std::move(sgdIncrementalTrainer), multiEpochArguments);
            }
            case LossFunctionEnum::log:
            {
               // auto sgdIncrementalTrainer = ;
                return trainers::MakeMultiEpochTrainer(trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::LogLoss(lossArguments.lossFunctionParameter), sgdArguments), multiEpochArguments);
            }
            case LossFunctionEnum::hinge:
            {
                auto sgdIncrementalTrainer = trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::HingeLoss(), sgdArguments);
                return trainers::MakeMultiEpochTrainer(std::move(sgdIncrementalTrainer), multiEpochArguments);
            }
            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }
}