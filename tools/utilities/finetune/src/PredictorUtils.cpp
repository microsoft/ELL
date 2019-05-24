////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PredictorUtils.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PredictorUtils.h"

namespace ell
{
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

// Explicit instantiations
template WeightsAndBias<float> GetWeightsAndBias(const ScalarPredictor& predictor);
template WeightsAndBias<double> GetWeightsAndBias(const ScalarPredictor& predictor);
template WeightsAndBias<float> GetWeightsAndBias(const VectorPredictor& predictor);
template WeightsAndBias<double> GetWeightsAndBias(const VectorPredictor& predictor);
} // namespace ell
