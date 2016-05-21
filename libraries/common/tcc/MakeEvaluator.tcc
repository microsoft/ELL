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

// evaluators
#include "Evaluator.h"
#include "BinaryErrorAggregator.h"
#include "LossAggregator.h"
#include "AUCAggregator.h"

namespace common
{
    template<typename PredictorType>
    std::shared_ptr<evaluators::IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const evaluators::EvaluatorParameters& evaluatorParameters, const LossArguments& lossArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch(lossArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            return evaluators::MakeEvaluator<PredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::SquaredLoss()));

        case LossFunctionEnum::log:
            return evaluators::MakeEvaluator<PredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::LogLoss(lossArguments.lossFunctionParameter)));

        case LossFunctionEnum::hinge:
            return evaluators::MakeEvaluator<PredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::HingeLoss()));

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this evaluator");
        }
    }

}