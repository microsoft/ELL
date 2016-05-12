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
    std::unique_ptr<trainers::IStochasticGradientDescentTrainer> MakeStochasticGradientDescentTrainer(uint64_t dim, const trainers::IStochasticGradientDescentTrainer::Parameters& parameters, const LossArguments& lossArguments)
    {

        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return std::make_unique<trainers::StochasticGradientDescentTrainer<lossFunctions::SquaredLoss>>(dim, parameters, lossFunctions::SquaredLoss());

        case LossFunctionEnum::log:
            return std::make_unique<trainers::StochasticGradientDescentTrainer<lossFunctions::LogLoss>>(dim, parameters, lossFunctions::LogLoss(lossArguments.lossFunctionParameter));

        case LossFunctionEnum::hinge:
            return std::make_unique<trainers::StochasticGradientDescentTrainer<lossFunctions::HingeLoss>>(dim, parameters, lossFunctions::HingeLoss());

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }

    std::unique_ptr<trainers::ISortingTreeTrainer> MakeSortingTreeTrainer(const trainers::ISortingTreeTrainer::Parameters & parameters, const LossArguments& lossArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return std::make_unique<trainers::SortingTreeTrainer<lossFunctions::SquaredLoss>>(parameters, lossFunctions::SquaredLoss());

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
        }
    }
}