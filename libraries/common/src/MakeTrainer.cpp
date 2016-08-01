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

// trainers
#include "HistogramForestTrainer.h"
#include "SortingForestTrainer.h"
#include "LogitBooster.h"

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

    std::unique_ptr<trainers::IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeSortingForestTrainer(const LossArguments& lossArguments, const ForestTrainerArguments& forestTrainerArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSortingForestTrainer(lossFunctions::SquaredLoss(), trainers::LogitBooster(), forestTrainerArguments);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::IIncrementalTrainer<predictors::SimpleForestPredictor>> MakeHistogramForestTrainer(const LossArguments& lossArguments, const ForestTrainerArguments& forestTrainerArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeHistogramForestTrainer(lossFunctions::SquaredLoss(), trainers::LogitBooster(), forestTrainerArguments);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }
}