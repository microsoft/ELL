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
    std::unique_ptr<trainers::IIncrementalTrainer<predictors::LinearPredictor>> MakeSGDIncrementalTrainer(uint64_t dim, const LossArguments& lossArguments, const SGDIncrementalTrainerArguments& sgdArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::SquaredLoss(), sgdArguments);

        case LossFunctionEnum::log:
            return trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::LogLoss(lossArguments.lossFunctionParameter), sgdArguments);

        case LossFunctionEnum::hinge:
            return trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::HingeLoss(), sgdArguments);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::IIncrementalTrainer<predictors::LinearPredictor>> MakeMultiEpochSGDIncrementalTrainer(uint64_t dim, const LossArguments& lossArguments, const SGDIncrementalTrainerArguments& sgdArguments, const MultiEpochIncrementalTrainerArguments& multiEpochArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
        {
            auto sgdIncrementalTrainer = trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::SquaredLoss(), sgdArguments);
            return trainers::MakeMultiEpochIncrementalTrainer(std::move(sgdIncrementalTrainer), multiEpochArguments);
        }
        case LossFunctionEnum::log:
        {
            // auto sgdIncrementalTrainer = ;
            return trainers::MakeMultiEpochIncrementalTrainer(trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::LogLoss(lossArguments.lossFunctionParameter), sgdArguments), multiEpochArguments);
        }
        case LossFunctionEnum::hinge:
        {
            auto sgdIncrementalTrainer = trainers::MakeSGDIncrementalTrainer(dim, lossFunctions::HingeLoss(), sgdArguments);
            return trainers::MakeMultiEpochIncrementalTrainer(std::move(sgdIncrementalTrainer), multiEpochArguments);
        }
        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::DecisionTreePredictor>> MakeSortingTreeTrainer(const LossArguments& lossArguments, const SortingTreeTrainerArguments& sortingTreeArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSortingTreeTrainer(lossFunctions::SquaredLoss(), sortingTreeArguments);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }
}