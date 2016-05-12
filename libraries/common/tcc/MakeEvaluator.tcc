////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MakeEvaluator.tcc (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////


// LossFunction
#include "SquaredLoss.h"
#include "LogLoss.h"
#include "HingeLoss.h"

namespace common
{
    template<typename PredictorType>
    std::unique_ptr<utilities::IBinaryClassificationEvaluator<PredictorType>> MakeBinaryClassificationEvaluator(const LossArguments& lossArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return std::make_unique<utilities::BinaryClassificationEvaluator<PredictorType, lossFunctions::SquaredLoss>>(lossFunctions::SquaredLoss());

        case LossFunctionEnum::log:
            return std::make_unique<utilities::BinaryClassificationEvaluator<PredictorType, lossFunctions::LogLoss>>(lossFunctions::LogLoss(lossArguments.lossFunctionParameter));

        case LossFunctionEnum::hinge:
            return std::make_unique<utilities::BinaryClassificationEvaluator<PredictorType, lossFunctions::HingeLoss>>(lossFunctions::HingeLoss());

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this evaluator");
        }
    }

}