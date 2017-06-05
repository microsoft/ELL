////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformedDataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataVector.h"

namespace ell
{
namespace data
{
    /// <summary> A class that holds a const reference to a data vector and a transformation. </summary>
    template <IterationPolicy policy, typename DataVectorType, typename TransformationType>
    class TransformedDataVector
    {
    public:
        /// <summary> Constructor. </summary>
        ///
        /// <param name="dataVector"> The data vector. </param>
        /// <param name="transformation"> The transformation. </param>
        TransformedDataVector(const DataVectorType& dataVector, TransformationType transformation);

        /// <summary> Gets a const reference to the data vector. </summary>
        ///
        /// <returns> The data vector. </returns>
        const DataVectorType& GetDataVector() const { return _dataVector; }

        /// <summary> Gets the transformation. </summary>
        ///
        /// <returns> The transformation. </returns>
        TransformationType GetTransformation() const { return _transformation; }

    private:
        const DataVectorType& _dataVector;
        TransformationType _transformation;
    };

    /// <summary> Helper function that constructs a TransformedDataVector. </summary>
    ///
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <typeparam name="DataVectorType"> The data vector type. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="dataVector"> The vector. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> A TransformedDataVector. </returns>
    template <IterationPolicy policy, typename DataVectorType, typename TransformationType>
    TransformedDataVector<policy, DataVectorType, TransformationType> MakeTransformedDataVector(const DataVectorType& dataVector, TransformationType transformation);

    /// <summary> Operator that adds a TransformedDataVector to a math::RowVector. </summary>
    ///
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <typeparam name="DataVectorType"> The data vector type. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="vector"> The math::RowVector being modified. </param>
    /// <param name="transformedDataVector"> The TransformedDataVector being added to vector. </param>
    template <IterationPolicy policy, typename DataVectorType, typename TransformationType>
    void operator+=(math::RowVectorReference<double> vector, const TransformedDataVector<policy, DataVectorType, TransformationType>& transformedDataVector);
}
}

#include "../tcc/TransformedDataVector.tcc"