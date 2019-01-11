////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationUtils.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataUtils.h"

#include <optimization/include/MatrixSolution.h>
#include <optimization/include/SDCAOptimizer.h>
#include <optimization/include/VectorSolution.h>

#include <optional>
#include <string>

struct FineTuneOptimizationParameters
{
    ell::optimization::SDCAOptimizerParameters optimizerParameters;
    std::optional<double> l1Regularization = 0.0;
    int maxEpochs = 0;
    double desiredPrecision = 0;
    bool normalizeInputs = false;
    bool normalizeOutputs = false;
    std::string randomSeed;
};

template <typename ElementType>
struct WeightsAndBias
{
    ell::math::RowMatrix<ElementType> weights = { 0, 0 };
    ell::math::ColumnVector<ElementType> bias = { 0 };
};

template <typename SolutionType>
struct OptimizerResult
{
    SolutionType predictor;
    ell::optimization::SDCASolutionInfo info;
};

using ScalarPredictor = ell::optimization::BiasedVectorSolution<float>;
using VectorPredictor = ell::optimization::BiasedMatrixSolution<float>;

template <typename ElementType>
WeightsAndBias<ElementType> GetWeightsAndBias(const ScalarPredictor& predictor);

template <typename ElementType>
WeightsAndBias<ElementType> GetWeightsAndBias(const VectorPredictor& predictor);

using ScalarOptimizerResult = OptimizerResult<ScalarPredictor>;
using VectorOptimizerResult = OptimizerResult<VectorPredictor>;

ScalarOptimizerResult TrainScalarPredictor(BinaryLabelDataContainer dataset,
                                           const FineTuneOptimizationParameters& optimizerParameters);

VectorOptimizerResult TrainVectorPredictor(VectorLabelDataContainer dataset,
                                           const FineTuneOptimizationParameters& optimizerParameters);
