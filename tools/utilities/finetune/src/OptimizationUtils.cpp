////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationUtils.cpp (utilities)
//  Authors:  Byron Changuion, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizationUtils.h"

#include <optimization/include/ElasticNetRegularizer.h>
#include <optimization/include/L2Regularizer.h>
#include <optimization/include/MatrixSolution.h>
#include <optimization/include/MultivariateLoss.h>
#include <optimization/include/SDCAOptimizer.h>
#include <optimization/include/SquareLoss.h>

#include <utilities/include/Logger.h>

#include <cmath>
#include <ios>
#include <limits>

using namespace ell;
using namespace logging;

// Prototypes
template <typename SolutionType, typename LossType, typename DatasetType>
OptimizerResult<SolutionType> TrainPredictor(std::shared_ptr<DatasetType> examples, LossType loss, const FineTuneOptimizationParameters& optimizerParameters);

template <typename SolutionType, typename LossType, typename DatasetType, typename RegularizerType>
OptimizerResult<SolutionType> TrainPredictor(std::shared_ptr<DatasetType> examples, LossType loss, RegularizerType regularizer, const FineTuneOptimizationParameters& optimizerParameters);

template <typename OptimizerType>
void RunOptimizer(OptimizerType& optimizer, int maxEpochs, double desiredPrecision);

template <typename ElementType>
WeightsAndBias<ElementType> GetWeightsAndBias(const ScalarPredictor& predictor)
{
    auto predictorWeights = predictor.GetVector();
    auto predictorBias = predictor.GetBias();
    WeightsAndBias<ElementType> result;
    result.weights = math::RowMatrix<ElementType>(1, predictorWeights.Size());
    result.bias = math::ColumnVector<ElementType>(1);

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
    result.weights = math::RowMatrix<ElementType>(predictorWeights.NumRows(), predictorWeights.NumColumns());
    result.bias = math::ColumnVector<ElementType>(predictorWeights.NumRows());

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

void PrintSDCAPredictorInfoHeader()
{
    Log() << "\tPrimal Objective";
    Log() << "\tDual Objective";
    Log() << "\tDuality gap";
    Log() << EOL;
}

void PrintSDCAOptimizerInfoValues(const optimization::SDCASolutionInfo& info)
{
    auto originalPrecision = Log().precision(6);
    auto originalFlags = Log().setf(std::ios::fixed);

    Log() << "\t" << info.primalObjective;
    Log() << "\t\t" << info.dualObjective;
    Log() << "\t" << info.DualityGap() << EOL;

    Log().setf(originalFlags);
    Log().precision(originalPrecision);
}

ScalarOptimizerResult TrainScalarPredictor(BinaryLabelDataContainer dataset, const FineTuneOptimizationParameters& optimizerParameters)
{
    using SolutionType = ScalarPredictor;

    auto examples = std::make_shared<BinaryLabelDataContainer>(std::move(dataset));
    optimization::SquareLoss loss; // or: LogLoss loss?
    return TrainPredictor<SolutionType>(examples, loss, optimizerParameters);
}

VectorOptimizerResult TrainVectorPredictor(VectorLabelDataContainer dataset, const FineTuneOptimizationParameters& optimizerParameters)
{
    using SolutionType = VectorPredictor;
    auto examples = std::make_shared<VectorLabelDataContainer>(std::move(dataset));
    optimization::MultivariateLoss<optimization::SquareLoss> loss;
    return TrainPredictor<SolutionType>(examples, loss, optimizerParameters);
}

template <typename SolutionType, typename LossType, typename DatasetType>
OptimizerResult<SolutionType> TrainPredictor(std::shared_ptr<DatasetType> examples, LossType loss, const FineTuneOptimizationParameters& optimizerParameters)
{
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

template <typename SolutionType, typename LossType, typename DatasetType, typename RegularizerType>
OptimizerResult<SolutionType> TrainPredictor(std::shared_ptr<DatasetType> examples, LossType loss, RegularizerType regularizer, const FineTuneOptimizationParameters& optimizerParameters)
{
    auto optimizer = optimization::MakeSDCAOptimizer<SolutionType>(examples, loss, regularizer, optimizerParameters.optimizerParameters, optimizerParameters.randomSeed);
    RunOptimizer(optimizer, optimizerParameters.maxEpochs, optimizerParameters.desiredPrecision);
    return { optimizer.GetSolution(), optimizer.GetSolutionInfo() };
}

template <typename OptimizerType>
void RunOptimizer(OptimizerType& optimizer, int maxEpochs, double desiredPrecision)
{
    optimizer.Update(maxEpochs, desiredPrecision);
}

// Explicit instantiations
template WeightsAndBias<float> GetWeightsAndBias(const ScalarPredictor& predictor);
template WeightsAndBias<double> GetWeightsAndBias(const ScalarPredictor& predictor);
template WeightsAndBias<float> GetWeightsAndBias(const VectorPredictor& predictor);
template WeightsAndBias<double> GetWeightsAndBias(const VectorPredictor& predictor);
