////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MakeEvaluator.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TrainerArguments.h"

// dataset
#include "RowDataset.h"

// utilities
#include "Evaluator.h"
#include "IncrementalEvaluator.h"

// stl
#include <memory>

namespace emll
{
namespace common
{
    /// <summary> Makes an evaluator. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of predictor. </typeparam>
    /// <param name="exampleIterator"> An example iterator that represents a training set. </param>
    /// <param name="evaluatorParameters"> The evaluator parameters. </param>
    /// <param name="lossArguments"> The loss command line arguments. </param>
    ///
    /// <returns> A unique_ptr to an IEvaluator. </returns>
    template <typename PredictorType>
    std::shared_ptr<evaluators::IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const evaluators::EvaluatorParameters& evaluatorParameters, const LossArguments& lossArguments);

    /// <summary> Makes an incremental evaluator (used to evaluate ensembles). </summary>
    ///
    /// <typeparam name="PredictorType"> Type of predictor. </typeparam>
    /// <param name="exampleIterator"> An example iterator that represents a training set. </param>
    /// <param name="evaluatorParameters"> The evaluator parameters. </param>
    /// <param name="lossArguments"> The loss command line arguments. </param>
    ///
    /// <returns> A unique_ptr to an IEvaluator. </returns>
    template <typename PredictorType>
    std::shared_ptr<evaluators::IIncrementalEvaluator<PredictorType>> MakeIncrementalEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const evaluators::EvaluatorParameters& evaluatorParameters, const LossArguments& lossArguments);
}
}

#include "../tcc/MakeEvaluator.tcc"
