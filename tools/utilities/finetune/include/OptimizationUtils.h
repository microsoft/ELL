////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationUtils.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataUtils.h"

#include <optimization/include/ElasticNetRegularizer.h>
#include <optimization/include/GetSparseSolution.h>
#include <optimization/include/HingeLoss.h>
#include <optimization/include/HuberLoss.h>
#include <optimization/include/L2Regularizer.h>
#include <optimization/include/LogisticLoss.h>
#include <optimization/include/MaskedMatrixSolution.h>
#include <optimization/include/MatrixSolution.h>
#include <optimization/include/MultivariateLoss.h>
#include <optimization/include/SDCAOptimizer.h>
#include <optimization/include/SmoothedHingeLoss.h>
#include <optimization/include/SquareLoss.h>
#include <optimization/include/VectorSolution.h>

#include <utilities/include/Logger.h>

#include <cmath>
#include <limits>
#include <optional>
#include <string>

namespace ell
{
enum class LossFunction
{
    square,
    logistic,
    hinge,
    smoothedHinge,
    huber
};

std::string ToString(LossFunction loss);

enum class SparsifyMethod
{
    l1,
    threshold,
    random
};

std::string ToString(SparsifyMethod method);

struct FineTuneOptimizationParameters
{
    // general
    std::string randomSeed;

    // optimization params
    bool optimizeFiltersIndependently = false;
    ell::optimization::SDCAOptimizerParameters optimizerParameters;
    int maxEpochs = 0;
    double desiredPrecision = 0;
    double requiredPrecision = 0;
    LossFunction lossFunction = LossFunction::square;
    std::optional<double> l1Regularization = 0.0;

    // sparsification
    bool reoptimizeSparseWeights = false;
    SparsifyMethod sparsifyMethod = SparsifyMethod::l1;
    double sparsityTarget = 0.0;
    double sparsityTargetEpsilon = 0.0;
};

struct FineTuneProblemParameters
{
    // random seed, normalization
    bool normalizeInputs = false;
    bool normalizeOutputs = false;

    FineTuneOptimizationParameters fineTuneParameters;
    FineTuneOptimizationParameters sparsifyParameters;
    FineTuneOptimizationParameters reoptimizeParameters;
    // copyParameters (== fineTune?)
};

template <typename ElementType>
struct WeightsAndBias
{
    ell::math::RowMatrix<ElementType> weights = { 0, 0 };
    ell::math::ColumnVector<ElementType> bias = { 0 };
};

struct SolutionInfo
{
    ell::optimization::SDCASolutionInfo info;
    double l2Regularization = 0;
    double l1Regularization = 0;
};

template <typename SolutionType>
struct OptimizerResult
{
    SolutionType predictor;
    SolutionInfo info;
    std::optional<SolutionInfo> failedOptInfo;
};

using ScalarPredictor = ell::optimization::BiasedVectorSolution<float>;
using VectorPredictor = ell::optimization::BiasedMatrixSolution<float>;

template <typename ElementType>
WeightsAndBias<ElementType> GetWeightsAndBias(const ScalarPredictor& predictor);

template <typename ElementType>
WeightsAndBias<ElementType> GetWeightsAndBias(const VectorPredictor& predictor);

using ScalarOptimizerResult = OptimizerResult<ScalarPredictor>;
using VectorOptimizerResult = OptimizerResult<VectorPredictor>;

// TODO: rename these to something without "train" and "predictor" in the name
ScalarOptimizerResult TrainScalarPredictor(BinaryLabelDataContainer dataset, const FineTuneOptimizationParameters& optimizerParameters);
VectorOptimizerResult TrainVectorPredictor(VectorLabelDataContainer dataset, const FineTuneOptimizationParameters& optimizerParameters);

// TODO: find a better (more general) way to indicate what the solution is, rather than with a "isSpatialConvolution" flag
VectorOptimizerResult TrainVectorPredictor(VectorLabelDataContainer dataset, const FineTuneOptimizationParameters& optimizerParameters, bool isSpatialConvolution);

VectorOptimizerResult ReoptimizeSparsePredictor(VectorOptimizerResult& sparseSolution, VectorLabelDataContainer dataset, const FineTuneOptimizationParameters& optimizerParameters, bool isSpatialConvolution);

#pragma region implementation

// Prototypes
template <typename SolutionType, typename LossType, typename DatasetType>
OptimizerResult<SolutionType> TrainPredictor(std::shared_ptr<DatasetType> examples, LossType loss, const FineTuneOptimizationParameters& optimizerParameters);

template <typename SolutionType, typename LossType, typename DatasetType, typename RegularizerType>
OptimizerResult<SolutionType> TrainPredictor(std::shared_ptr<DatasetType> examples, LossType loss, RegularizerType regularizer, const FineTuneOptimizationParameters& optimizerParameters);

template <typename SolutionType, typename LossType, typename DatasetType>
OptimizerResult<SolutionType> TrainSparsePredictor(std::shared_ptr<DatasetType> examples, LossType loss, ell::optimization::ElasticNetRegularizer regularizer, const FineTuneOptimizationParameters& optimizerParameters);

template <typename OptimizerType>
OptimizerResult<typename OptimizerType::Solution> OptimizeAndTruncate(OptimizerType& optimizer, const FineTuneOptimizationParameters& optimizerParameters);

template <typename OptimizerType>
void RunOptimizer(OptimizerType& optimizer, int maxEpochs, double desiredPrecision);

template <typename SolutionType>
double GetSparsity(OptimizerResult<SolutionType>& result);

// Implementations
template <typename ElementType>
WeightsAndBias<ElementType> GetWeightsAndBias(const ScalarPredictor& predictor)
{
    auto predictorWeights = predictor.GetVector();
    auto predictorBias = predictor.GetBias();
    WeightsAndBias<ElementType> result;
    result.weights = ell::math::RowMatrix<ElementType>(1, predictorWeights.Size());
    result.bias = ell::math::ColumnVector<ElementType>(1);

    // Set the weights matrix and bias from the predictors.
    // Each row in the weights is the learned weights from that predictor.
    // Each element in the bias is the learned bias from that predictor.
    for (size_t j = 0; j < predictorWeights.Size(); ++j)
    {
        result.weights(0, j) = predictorWeights[j];
    }
    result.bias[0] = predictorBias;
    return result;
}

template <typename ElementType>
WeightsAndBias<ElementType> GetWeightsAndBias(const VectorPredictor& predictor)
{
    auto predictorWeights = predictor.GetMatrix().Transpose();
    auto predictorBias = predictor.GetBias();
    WeightsAndBias<ElementType> result;
    result.weights = ell::math::RowMatrix<ElementType>(predictorWeights.NumRows(), predictorWeights.NumColumns());
    result.bias = ell::math::ColumnVector<ElementType>(predictorWeights.NumRows());

    // Set the weights matrix and bias from the predictors.
    // Each row in the weights is the learned weights from that predictor.
    // Each element in the bias is the learned bias from that predictor.
    for (size_t i = 0; i < predictorWeights.NumRows(); ++i)
    {
        for (size_t j = 0; j < predictorWeights.NumColumns(); ++j)
        {
            result.weights(i, j) = predictorWeights(i, j);
        }
        result.bias[i] = predictorBias[i];
    }
    return result;
}

template <typename SolutionType, typename LossType, typename DatasetType>
OptimizerResult<SolutionType> TrainPredictor(std::shared_ptr<DatasetType> examples, LossType loss, const FineTuneOptimizationParameters& optimizerParameters)
{
    bool sparsify = optimizerParameters.sparsityTarget > 0.0;
    if (sparsify)
    {
        return TrainSparsePredictor<SolutionType>(examples, loss, optimizerParameters);
    }

    if (optimizerParameters.l1Regularization > 0.0)
    {
        ell::optimization::ElasticNetRegularizer regularizer{ optimizerParameters.l1Regularization.value_or(0.0) };
        return TrainPredictor<SolutionType>(examples, loss, regularizer, optimizerParameters);
    }
    else
    {
        ell::optimization::L2Regularizer regularizer;
        return TrainPredictor<SolutionType>(examples, loss, regularizer, optimizerParameters);
    }
}

template <typename SolutionType, typename LossType, typename DatasetType>
OptimizerResult<SolutionType> TrainSparsePredictor(std::shared_ptr<DatasetType> examples, LossType loss, const FineTuneOptimizationParameters& optimizerParameters)
{
    using namespace ell::logging;

    if (optimizerParameters.sparsifyMethod == SparsifyMethod::l1)
    {
        double targetDensity = 1.0 - optimizerParameters.sparsityTarget;
        ell::optimization::GetSparseSolutionParameters sparseParams;
        sparseParams.targetDensity = { targetDensity - (optimizerParameters.sparsityTargetEpsilon / 2), targetDensity + (optimizerParameters.sparsityTargetEpsilon / 2) };
        sparseParams.SDCAParameters = optimizerParameters.optimizerParameters;
        sparseParams.maxEpochs = optimizerParameters.maxEpochs * 20; // ???
        sparseParams.SDCAMaxEpochsPerCall = optimizerParameters.maxEpochs;
        sparseParams.SDCAEarlyExitDualityGap = optimizerParameters.desiredPrecision;
        sparseParams.SDCARandomSeedString = optimizerParameters.randomSeed;
        sparseParams.exponentialSearchGuess = 1.0 / 64.0;
        sparseParams.exponentialSearchBase = 4.0;

        // Run the optimizer to find a sparse solution
        Log() << "Finding a sparse solution:\n";
        auto sparseSolution = ell::optimization::GetSparseSolution<SolutionType>(examples, loss, sparseParams);
        Log() << "GetSparseSolution found solution with sparsity " << (1.0 - sparseSolution.density) << ", using L1 parameter: " << sparseSolution.beta << ", and duality gap: " << sparseSolution.info.DualityGap() << std::endl;

        OptimizerResult<SolutionType> result{ sparseSolution.solution, { sparseSolution.info, optimizerParameters.optimizerParameters.regularizationParameter, sparseSolution.beta }, {} };
        return result;
    }
    else if (optimizerParameters.sparsifyMethod == SparsifyMethod::threshold)
    {
        ell::optimization::L2Regularizer regularizer;
        auto thresholdParameters = optimizerParameters;
        thresholdParameters.sparsityTarget = 0;
        thresholdParameters.l1Regularization.reset();
        auto solution = TrainPredictor<SolutionType>(examples, loss, regularizer, thresholdParameters);
        auto getSparsity = [&](double threshold) {
            int numZeros = 0;
            solution.predictor.GetVector().Visit([&](double val) {
                if (std::abs(val) <= threshold)
                {
                    ++numZeros;
                }
            });
            return static_cast<double>(numZeros) / solution.predictor.GetVector().Size();
        };
        ell::optimization::Interval targetSparsityInterval = { optimizerParameters.sparsityTarget - (optimizerParameters.sparsityTargetEpsilon / 2), optimizerParameters.sparsityTarget + (optimizerParameters.sparsityTargetEpsilon / 2) };
        double bestThreshold = 0;
        auto initialSparsity = getSparsity(bestThreshold);

        // Only threshold the solution if the un-thresholded solution isn't already sparse enough
        if (initialSparsity < targetSparsityInterval.Begin())
        {
            ell::optimization::BinarySearchParameters binarySearchParameters{ targetSparsityInterval, { 0, 1.0 }, false, {} };
            auto binarySearch = ell::optimization::BinarySearch(getSparsity, binarySearchParameters);
            const int maxCalls = 100;
            binarySearch.Update(maxCalls);

            bestThreshold = binarySearch.GetCurrentSearchInterval().Begin();

            // threshold solution
            auto weights = solution.predictor.GetVector();
            weights.Transform([&](double val) {
                return (std::abs(val) <= bestThreshold) ? 0 : val;
            });
        }

        auto finalSparsity = GetSparsity(solution.predictor);
        Log() << "Found a sparse solution with sparsity " << finalSparsity << ", using threshold: " << bestThreshold << std::endl;

        OptimizerResult<SolutionType> result{ solution.predictor, solution.info, {} };
        return result;
    }
    else if (optimizerParameters.sparsifyMethod == SparsifyMethod::random)
    {
        ell::optimization::L2Regularizer regularizer;
        auto randomParameters = optimizerParameters;
        randomParameters.sparsityTarget = 0;
        randomParameters.l1Regularization.reset();
        auto solution = TrainPredictor<SolutionType>(examples, loss, regularizer, randomParameters);
        auto weights = solution.predictor.GetVector();
        std::vector<int> indices(weights.Size());
        std::iota(indices.begin(), indices.end(), 0);
        std::default_random_engine randomEngine;
        std::seed_seq seed(randomParameters.randomSeed.begin(), randomParameters.randomSeed.end());
        randomEngine.seed(seed);
        std::shuffle(indices.begin(), indices.end(), randomEngine);
        indices.resize(optimizerParameters.sparsityTarget * indices.size());
        for (auto index : indices)
        {
            weights[index] = 0;
        }
        auto finalSparsity = GetSparsity(solution.predictor);
        Log() << "Found a sparse solution with sparsity " << finalSparsity << ", by randomly zeroing weights" << std::endl;

        OptimizerResult<SolutionType> result{ solution.predictor, solution.info, {} };
        return result;
    }
    else
    {
        throw ell::utilities::LogicException(ell::utilities::LogicExceptionErrors::illegalState, "Error: unknown sparsification method");
    }
}

template <typename SolutionType, typename LossType, typename DatasetType>
OptimizerResult<SolutionType> ReoptimizeSparsePredictor(OptimizerResult<SolutionType> sparseSolution, std::shared_ptr<DatasetType> examples, LossType loss, const FineTuneOptimizationParameters& optimizerParameters)
{
    using namespace ell::logging;
    if constexpr (std::is_same_v<SolutionType, ell::optimization::MatrixSolution<float, true>> ||
                  std::is_same_v<SolutionType, ell::optimization::MatrixSolution<float, false>> ||
                  std::is_same_v<SolutionType, ell::optimization::MatrixSolution<double, true>> ||
                  std::is_same_v<SolutionType, ell::optimization::MatrixSolution<double, false>>)
    {
        using MaskedSolutionType = typename ell::optimization::MaskedMatrixSolution<SolutionType>;
        using WeightsType = typename SolutionType::WeightsType;
        using MaskType = typename MaskedSolutionType::MaskType;

        const auto& weights = sparseSolution.predictor.GetMatrix();
        auto numRows = weights.NumRows();
        auto numColumns = weights.NumColumns();
        auto frozenWeights = WeightsType(numRows, numColumns);
        frozenWeights.Fill(0);
        MaskType mask(weights.NumRows(), weights.NumColumns());
        for (size_t i = 0; i < numRows; ++i)
        {
            for (size_t j = 0; j < numColumns; ++j)
            {
                mask(i, j) = (weights(i, j) == 0) ? 1 : 0;
            }
        }

        // Create optimizer using a MaskedMatrixSolution, run it, and use its base solution
        ell::optimization::L2Regularizer regularizer;
        auto maskedOptimizer = ell::optimization::MakeSDCAOptimizer<MaskedSolutionType>(examples, loss, regularizer, optimizerParameters.optimizerParameters, { mask, frozenWeights }, optimizerParameters.randomSeed);
        auto result = OptimizeAndTruncate(maskedOptimizer, optimizerParameters);

        Log() << "reoptimized duality gap: " << result.info.info.DualityGap() << std::endl;
        return OptimizerResult<SolutionType>{ result.predictor.GetBaseSolution(), result.info, {} };
    }
    return sparseSolution;
}

template <typename SolutionType, typename LossType, typename DatasetType, typename RegularizerType>
OptimizerResult<SolutionType> TrainPredictor(std::shared_ptr<DatasetType> examples, LossType loss, RegularizerType regularizer, const FineTuneOptimizationParameters& optimizerParameters)
{
    bool hasL1 = optimizerParameters.l1Regularization.value_or(0.0) != 0;
    if (!std::is_same_v<RegularizerType, ell::optimization::ElasticNetRegularizer> && (hasL1 || optimizerParameters.sparsityTarget != 0))
    {
        throw ell::utilities::LogicException(ell::utilities::LogicExceptionErrors::illegalState, "Error: not using ElasticNet regularizer when L1 regularization or sparsity target is nonzero");
    }

    auto optimizer = ell::optimization::MakeSDCAOptimizer<SolutionType>(examples, loss, regularizer, optimizerParameters.optimizerParameters, optimizerParameters.randomSeed);
    return OptimizeAndTruncate(optimizer, optimizerParameters);
}

template <typename OptimizerType>
OptimizerResult<typename OptimizerType::Solution> OptimizeAndTruncate(OptimizerType& optimizer, const FineTuneOptimizationParameters& optimizerParameters)
{
    RunOptimizer(optimizer, optimizerParameters.maxEpochs, optimizerParameters.desiredPrecision);
    OptimizerResult<typename OptimizerType::Solution> result = { optimizer.GetSolution(), { optimizer.GetSolutionInfo(), optimizerParameters.optimizerParameters.regularizationParameter, optimizerParameters.l1Regularization.value_or(0.0) }, {} };
    return result;
}

template <typename OptimizerType>
void RunOptimizer(OptimizerType& optimizer, int maxEpochs, double desiredPrecision)
{
    optimizer.Update(maxEpochs, desiredPrecision);
}

template <typename SolutionType>
double GetSparsity(OptimizerResult<SolutionType>& result)
{
    auto vec = result.predictor.GetVector();
    auto nnz = vec.Norm0();
    return static_cast<double>(vec.Size() - nnz) / static_cast<double>(vec.Size());
}
#pragma endregion implementation
} // namespace ell