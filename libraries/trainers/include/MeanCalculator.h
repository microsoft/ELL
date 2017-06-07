////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MeanCalculator.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// data
#include "Dataset.h"

// math
#include "Vector.h"

namespace ell
{
namespace trainers
{
    /// <summary> Applies a sparse or dense transformation to each data vector in a dataset and returns the mean </summary>
    ///
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="anyDataset"> The dataset. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> The calculated mean. </returns>
    template<data::IterationPolicy policy, typename TransformationType>
    math::RowVector<double> CalculateTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation);

    /// <summary> Applies a sparse transformation to each data vector in a dataset and returns the mean </summary>
    ///
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="anyDataset"> The dataset. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> The calculated mean. </returns>
    template<typename TransformationType>
    math::RowVector<double> CalculateSparseTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation);

    /// <summary> Applies a dense transformation to each data vector in a dataset and returns the mean </summary>
    ///
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="anyDataset"> The dataset. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> The calculated mean. </returns>
    template<typename TransformationType>
    math::RowVector<double> CalculateDenseTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation);

    /// <summary> Calcluates the mean of data vectors in a dataset </summary>
    ///
    /// <param name="anyDataset"> The dataset. </param>
    ///
    /// <returns> The calculated mean. </returns>
    math::RowVector<double> CalculateMean(const data::AnyDataset& anyDataset);
}
}

#include "../tcc/MeanCalculator.tcc"