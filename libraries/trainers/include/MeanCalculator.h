////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MeanCalculator.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <data/include/Dataset.h>

#include <math/include/Vector.h>

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
    template <data::IterationPolicy policy, typename TransformationType>
    math::RowVector<double> CalculateTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation);

    /// <summary> Applies a sparse transformation to each data vector in a dataset and returns the mean </summary>
    ///
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="anyDataset"> The dataset. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> The calculated mean. </returns>
    template <typename TransformationType>
    math::RowVector<double> CalculateSparseTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation);

    /// <summary> Applies a dense transformation to each data vector in a dataset and returns the mean </summary>
    ///
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="anyDataset"> The dataset. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> The calculated mean. </returns>
    template <typename TransformationType>
    math::RowVector<double> CalculateDenseTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation);

    /// <summary> Calcluates the mean of data vectors in a dataset </summary>
    ///
    /// <param name="anyDataset"> The dataset. </param>
    ///
    /// <returns> The calculated mean. </returns>
    math::RowVector<double> CalculateMean(const data::AnyDataset& anyDataset);
} // namespace trainers
} // namespace ell

#pragma region implementation

#include <data/include/TransformedDataVector.h>

namespace ell
{
namespace trainers
{
    template <data::IterationPolicy policy, typename TransformationType>
    math::RowVector<double> CalculateTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation)
    {
        // get example iterator
        auto exampleIterator = anyDataset.GetExampleIterator<data::AutoSupervisedExample>();

        math::RowVector<double> result;
        size_t count = 0;
        while (exampleIterator.IsValid())
        {
            const auto& x = exampleIterator.Get().GetDataVector();
            if (x.PrefixLength() > result.Size())
            {
                result.Resize(x.PrefixLength());
            }

            result += data::MakeTransformedDataVector<policy>(x, transformation);
            ++count;
            exampleIterator.Next();
        }

        double scale = 1.0 / count;
        result.Transform([scale](double x) { return scale * x; });

        return result;
    }

    template <typename TransformationType>
    math::RowVector<double> CalculateSparseTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation)
    {
        return CalculateTransformedMean<data::IterationPolicy::skipZeros>(anyDataset, transformation);
    }

    template <typename TransformationType>
    math::RowVector<double> CalculateDenseTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation)
    {
        return CalculateTransformedMean<data::IterationPolicy::all>(anyDataset, transformation);
    }
} // namespace trainers
} // namespace ell

#pragma endregion implementation
