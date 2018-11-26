////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MakeEvaluator.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TrainerArguments.h"

#include <data/include/Dataset.h>

#include <evaluators/include/Evaluator.h>
#include <evaluators/include/IncrementalEvaluator.h>

#include <memory>

namespace ell
{
namespace common
{
    /// <summary> Makes an evaluator. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of predictor. </typeparam>
    /// <param name="anyDataset"> A dataset. </param>
    /// <param name="evaluatorParameters"> The evaluator parameters. </param>
    /// <param name="lossFunctionArguments"> The loss command line arguments. </param>
    ///
    /// <returns> A unique_ptr to an IEvaluator. </returns>
    template <typename PredictorType>
    std::shared_ptr<evaluators::IEvaluator<PredictorType>> MakeEvaluator(const data::AnyDataset& anyDataset, const evaluators::EvaluatorParameters& evaluatorParameters, const LossFunctionArguments& lossFunctionArguments);

    /// <summary> Makes an incremental evaluator (used to evaluate ensembles). </summary>
    ///
    /// <typeparam name="PredictorType"> Type of predictor. </typeparam>
    /// <param name="exampleIterator"> An example iterator that represents a training set. </param>
    /// <param name="evaluatorParameters"> The evaluator parameters. </param>
    /// <param name="lossFunctionArguments"> The loss command line arguments. </param>
    ///
    /// <returns> A unique_ptr to an IEvaluator. </returns>
    template <typename PredictorType>
    std::shared_ptr<evaluators::IIncrementalEvaluator<PredictorType>> MakeIncrementalEvaluator(data::AutoSupervisedExampleIterator exampleIterator, const evaluators::EvaluatorParameters& evaluatorParameters, const LossFunctionArguments& lossFunctionArguments);
} // namespace common
} // namespace ell

#pragma region implementation

// Loss functions
#include <functions/include/HingeLoss.h>
#include <functions/include/LogLoss.h>
#include <functions/include/SquaredLoss.h>

#include <evaluators/include/AUCAggregator.h>
#include <evaluators/include/BinaryErrorAggregator.h>
#include <evaluators/include/LossAggregator.h>

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
} // namespace common
} // namespace ell

#pragma endregion implementation
