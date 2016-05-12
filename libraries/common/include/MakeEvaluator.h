////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MakeEvaluator.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TrainerArguments.h"

// stl
#include <memory>

// utilities
#include "BinaryClassificationEvaluator.h"

namespace common
{
    /// <summary> Makes a binary classification evaluator. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of predictor. </typeparam>
    /// <param name="lossArguments"> The loss command line arguments. </param>
    ///
    /// <returns>
    /// A unique_ptr to an IBinaryClassificationEvaluator
    /// </returns>
    template<typename PredictorType>
    std::unique_ptr<utilities::IBinaryClassificationEvaluator<PredictorType>> MakeBinaryClassificationEvaluator(const LossArguments& lossArguments);
}

#include "../tcc/MakeEvaluator.tcc"
