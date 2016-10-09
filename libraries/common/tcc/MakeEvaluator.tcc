////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MakeEvaluator.tcc (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// LossFunction
#include "HingeLoss.h"
#include "LogLoss.h"
#include "SquaredLoss.h"

// evaluators
#include "AUCAggregator.h"
#include "BinaryErrorAggregator.h"
#include "LossAggregator.h"

namespace emll
{
namespace common
{
    template <typename PredictorType>
    std::shared_ptr<evaluators::IEvaluator<PredictorType>> MakeEvaluator(data::AnyDataset anyDataset, const evaluators::EvaluatorParameters& evaluatorParameters, const LossArguments& lossArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
            case LossFunctionEnum::squared:
                return evaluators::MakeEvaluator<PredictorType>(anyDataset, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::SquaredLoss()));

            case LossFunctionEnum::log:
                return evaluators::MakeEvaluator<PredictorType>(anyDataset, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::LogLoss(lossArguments.lossFunctionParameter)));

            case LossFunctionEnum::hinge:
                return evaluators::MakeEvaluator<PredictorType>(anyDataset, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::HingeLoss()));

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this evaluator");
        }
    }

    template <typename BasePredictorType>
    std::shared_ptr<evaluators::IIncrementalEvaluator<BasePredictorType>> MakeIncrementalEvaluator(data::ExampleIterator<data::AutoSupervisedExample> exampleIterator, const evaluators::EvaluatorParameters& evaluatorParameters, const LossArguments& lossArguments)
    {
        using LossFunctionEnum = common::LossArguments::LossFunction;

        switch (lossArguments.lossFunction)
        {
            case LossFunctionEnum::squared:
                return evaluators::MakeIncrementalEvaluator<BasePredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::SquaredLoss()));

            case LossFunctionEnum::log:
                return evaluators::MakeIncrementalEvaluator<BasePredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::LogLoss(lossArguments.lossFunctionParameter)));

            case LossFunctionEnum::hinge:
                return evaluators::MakeIncrementalEvaluator<BasePredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(lossFunctions::HingeLoss()));

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this evaluator");
        }
    }
}
}
