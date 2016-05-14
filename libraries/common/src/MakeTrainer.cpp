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
    std::unique_ptr<trainers::ILearner<predictors::LinearPredictor>> MakeStochasticGradientDescentLearner(uint64_t dim, const trainers::StochasticGradientDescentLearnerParameters& parameters, const LossArguments& lossArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return MakeStochasticGradientDescentLearner(dim, parameters, lossFunctions::SquaredLoss());

        case LossFunctionEnum::log:
            return MakeStochasticGradientDescentLearner(dim, parameters, lossFunctions::LogLoss(lossArguments.lossFunctionParameter));

        case LossFunctionEnum::hinge:
            return MakeStochasticGradientDescentLearner(dim, parameters, lossFunctions::HingeLoss());

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ITrainer<predictors::DecisionTreePredictor>> MakeSortingTreeTrainer(const trainers::SortingTreeTrainerParameters & parameters, const LossArguments& lossArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return MakeSortingTreeTrainer(parameters, lossFunctions::SquaredLoss());

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }
}