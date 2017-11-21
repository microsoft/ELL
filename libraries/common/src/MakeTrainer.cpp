////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MakeTrainer.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MakeTrainer.h"

// functions
#include "HingeLoss.h"
#include "L2Regularizer.h"
#include "LogLoss.h"
#include "SmoothHingeLoss.h"
#include "SquaredLoss.h"

// utilities
#include "CommandLineParser.h"

// trainers
#include "HistogramForestTrainer.h"
#include "LogitBooster.h"
#include "SortingForestTrainer.h"
#include "ThresholdFinder.h"
#include "ProtoNNTrainer.h"

namespace ell
{
namespace common
{
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSGDTrainer(const LossFunctionArguments& lossFunctionArguments, const trainers::SGDTrainerParameters& trainerParameters)
    {
        using LossFunctionEnum = common::LossFunctionArguments::LossFunction;

        switch (lossFunctionArguments.lossFunction)
        {
            case LossFunctionEnum::squared:
                return trainers::MakeSGDTrainer(functions::SquaredLoss(), trainerParameters);

            case LossFunctionEnum::log:
                return trainers::MakeSGDTrainer(functions::LogLoss(), trainerParameters);

            case LossFunctionEnum::hinge:
                return trainers::MakeSGDTrainer(functions::HingeLoss(), trainerParameters);

            case LossFunctionEnum::smoothHinge:
                return trainers::MakeSGDTrainer(functions::SmoothHingeLoss(), trainerParameters);

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSparseDataSGDTrainer(const LossFunctionArguments& lossFunctionArguments, const trainers::SGDTrainerParameters& trainerParameters)
    {
        using LossFunctionEnum = common::LossFunctionArguments::LossFunction;

        switch (lossFunctionArguments.lossFunction)
        {
            case LossFunctionEnum::squared:
                return trainers::MakeSparseDataSGDTrainer(functions::SquaredLoss(), trainerParameters);

            case LossFunctionEnum::log:
                return trainers::MakeSparseDataSGDTrainer(functions::LogLoss(), trainerParameters);

            case LossFunctionEnum::hinge:
                return trainers::MakeSparseDataSGDTrainer(functions::HingeLoss(), trainerParameters);

            case LossFunctionEnum::smoothHinge:
                return trainers::MakeSparseDataSGDTrainer(functions::SmoothHingeLoss(), trainerParameters);

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSparseDataCenteredSGDTrainer(const LossFunctionArguments& lossFunctionArguments, math::RowVector<double> center, const trainers::SGDTrainerParameters& trainerParameters)
    {
        using LossFunctionEnum = common::LossFunctionArguments::LossFunction;

        switch (lossFunctionArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSparseDataCenteredSGDTrainer(functions::SquaredLoss(), std::move(center), trainerParameters);

        case LossFunctionEnum::log:
            return trainers::MakeSparseDataCenteredSGDTrainer(functions::LogLoss(), std::move(center), trainerParameters);

        case LossFunctionEnum::hinge:
            return trainers::MakeSparseDataCenteredSGDTrainer(functions::HingeLoss(), std::move(center), trainerParameters);

        case LossFunctionEnum::smoothHinge:
            return trainers::MakeSparseDataCenteredSGDTrainer(functions::SmoothHingeLoss(), std::move(center), trainerParameters);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSDCATrainer(const LossFunctionArguments& lossFunctionArguments, const trainers::SDCATrainerParameters& trainerParameters)
    {
        using LossFunctionEnum = common::LossFunctionArguments::LossFunction;

        switch (lossFunctionArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return trainers::MakeSDCATrainer(functions::SquaredLoss(), functions::L2Regularizer(), trainerParameters);

        case LossFunctionEnum::log:
            return trainers::MakeSDCATrainer(functions::LogLoss(), functions::L2Regularizer(), trainerParameters);

        case LossFunctionEnum::smoothHinge:
            return trainers::MakeSDCATrainer(functions::SmoothHingeLoss(), functions::L2Regularizer(), trainerParameters);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::SimpleForestPredictor>> MakeForestTrainer(const LossFunctionArguments& lossFunctionArguments, const ForestTrainerArguments& trainerArguments)
    {
        using LossFunctionEnum = common::LossFunctionArguments::LossFunction;

        switch (lossFunctionArguments.lossFunction)
        {
            case LossFunctionEnum::squared:

                if (trainerArguments.sortingTrainer)
                {
                    return trainers::MakeSortingForestTrainer(functions::SquaredLoss(), trainers::LogitBooster(), trainerArguments);
                }
                else
                {
                    return trainers::MakeHistogramForestTrainer(functions::SquaredLoss(), trainers::LogitBooster(), trainers::ExhaustiveThresholdFinder(), trainerArguments);
                }

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::ProtoNNPredictor>> MakeProtoNNTrainer(const trainers::ProtoNNTrainerParameters& parameters)
    {
        return trainers::MakeProtoNNTrainer(parameters);
    }
}
}
