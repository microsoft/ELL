////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MakeEvaluator.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TrainerArguments.h"

// data
#include "Dataset.h"

// utilities
#include "Evaluator.h"
#include "IncrementalEvaluator.h"

// stl
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
}
}

#include "../tcc/MakeEvaluator.tcc"
