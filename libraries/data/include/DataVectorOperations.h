////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataVectorOperations.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
# pragma once

#include "DataVector.h"
#include "TransformedDataVector.h"

namespace ell
{
namespace data
{
    /// <summary> Multiplication operator for scalar and DataVector </summary>
    ///
    /// <typeparam name="DataVectorType"> Data vector type. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The data vector. </param>
    ///
    /// <returns> The TransformedDataVector generated from the vector and the scaling operation. </returns>
    template <typename DataVectorType, IsDataVector<DataVectorType> Concept = true>
    auto operator*(double scalar, const DataVectorType& vector);

    /// <summary> Multiplication operator for scalar and DataVector </summary>
    ///
    /// <typeparam name="DataVectorType"> Data vector type. </typeparam>
    /// <param name="vector"> The data vector. </param>
    /// <param name="scalar"> The scalar. </param>
    ///
    /// <returns> The TransformedDataVector generated from the vector and the scaling operation. </returns>
    template <typename DataVectorType, IsDataVector<DataVectorType> Concept = true>
    auto operator*(const DataVectorType& vector, double scalar);

    /// <summary> Multiplication operator (dot product) for vector and data vector. </summary>
    ///
    /// <param name="vector"> The vector. </param>
    /// <param name="dataVector"> The data vector. </param>
    ///
    /// <returns> The result of the dot product. </returns>
    template <typename ElementType>
    ElementType operator*(math::UnorientedConstVectorBase<ElementType> vector, const IDataVector& dataVector);

    /// <summary> Multiplication operator (dot product) for vector and data vector. </summary>
    ///
    /// <param name="dataVector"> The data vector. </param>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The result of the dot product. </returns>
    double operator*(const IDataVector& dataVector, math::UnorientedConstVectorBase<double> vector);

    /// <summary> Elementwise square operation for data vectors. </summary>
    ///
    /// <typeparam name="DataVectorType"> Data vector type. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedDataVector generated from the vector and the elementwise square operation. </returns>
    template <typename DataVectorType>
    auto Square(const DataVectorType& vector);

    /// <summary> Elementwise square-root operation for data vectors. </summary>
    ///
    /// <typeparam name="DataVectorType"> Data vector type. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedDataVector generated from the vector and the elementwise square-root operation. </returns>
    template <typename DataVectorType>
    auto Sqrt(const DataVectorType& vector);

    /// <summary> Elementwise absolute value operation for data vectors. </summary>
    ///
    /// <typeparam name="DataVectorType"> Data vector type. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedDataVector generated from the vector and the elementwise absolute value operation. </returns>
    template <typename DataVectorType>
    auto Abs(const DataVectorType& vector);

    /// <summary> Elementwise zero indicator operation for data vectors. </summary>
    ///
    /// <typeparam name="DataVectorType"> Data vector type. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedDataVector generated from the vector and the elementwise zero indicator. </returns>
    template <typename DataVectorType>
    auto ZeroIndicator(const DataVectorType& vector);
}
}

#include "../tcc/DataVectorOperations.tcc"
