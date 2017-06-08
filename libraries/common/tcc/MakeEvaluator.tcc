////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
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

namespace ell
{
namespace common
{
    template <typename PredictorType>
    std::shared_ptr<evaluators::IEvaluator<PredictorType>> MakeEvaluator(const data::AnyDataset& anyDataset, const evaluators::EvaluatorParameters& evaluatorParameters, const LossFunctionArguments& lossFunctionArguments)
    {
        using LossFunctionEnum = common::LossFunctionArguments::LossFunction;

        switch (lossFunctionArguments.lossFunction)
        {
            case LossFunctionEnum::squared:
                return evaluators::MakeEvaluator<PredictorType>(anyDataset, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(functions::SquaredLoss()));

            case LossFunctionEnum::log:
                return evaluators::MakeEvaluator<PredictorType>(anyDataset, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(functions::LogLoss()));

            case LossFunctionEnum::hinge:
                return evaluators::MakeEvaluator<PredictorType>(anyDataset, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(functions::HingeLoss()));

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this evaluator");
        }
    }

    template <typename BasePredictorType>
    std::shared_ptr<evaluators::IIncrementalEvaluator<BasePredictorType>> MakeIncrementalEvaluator(data::AutoSupervisedExampleIterator exampleIterator, const evaluators::EvaluatorParameters& evaluatorParameters, const LossFunctionArguments& lossFunctionArguments)
    {
        using LossFunctionEnum = common::LossFunctionArguments::LossFunction;

        switch (lossFunctionArguments.lossFunction)
        {
            case LossFunctionEnum::squared:
                return evaluators::MakeIncrementalEvaluator<BasePredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(functions::SquaredLoss()));

            case LossFunctionEnum::log:
                return evaluators::MakeIncrementalEvaluator<BasePredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(functions::LogLoss()));

            case LossFunctionEnum::hinge:
                return evaluators::MakeIncrementalEvaluator<BasePredictorType>(exampleIterator, evaluatorParameters, evaluators::BinaryErrorAggregator(), evaluators::AUCAggregator(), evaluators::MakeLossAggregator(functions::HingeLoss()));

            default:
                throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this evaluator");
        }
    }
}
}
