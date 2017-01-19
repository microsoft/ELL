////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVectorOperators.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataVector.h"

// math
#include "Vector.h"

namespace ell
{
namespace data
{
    /// <summary> A class that holds a const reference to a data vector and a scalar. </summary>
    class ScaledDataVector
    {
    public:
        /// <summary> Constructor. </summary>
        ///
        /// <param name="scalar"> The scalar. </param>
        /// <param name="dataVector"> The data vector. </param>
        ScaledDataVector(double scalar, const IDataVector& dataVector);

        /// <summary> Gets the scalar. </summary>
        ///
        /// <returns> The scalar. </returns>
        double GetScalar() const { return _scalar; }

        /// <summary> Gets a const reference to the data vector. </summary>
        ///
        /// <returns> The data vector. </returns>
        const IDataVector& GetDataVector() const { return _dataVector; }

    private:
        double _scalar;
        const IDataVector& _dataVector;
    };

    /// <summary> Creates a ScaledDataVector from a scalar and a data vector. </summary>
    ///
    /// <param name="scalar"> The scalar. </param>
    /// <param name="dataVector"> The data vector. </param>
    ///
    /// <returns> A ScaledDataVector that represents the result of the operation. </returns>
    ScaledDataVector operator*(double scalar, const IDataVector& dataVector);

    /// <summary> Adds a ScaledDataVector to a math::RowVector. </summary>
    ///
    /// <param name="vector"> The math::RowVector being modified. </param>
    /// <param name="scaledDataVector"> The ScaledDataVector being added to the vector. </param>
    void operator+=(math::RowVectorReference<double> vector, const ScaledDataVector& scaledDataVector);

    /// <summary> Adds a DataVector to a math::RowVector. </summary>
    ///
    /// <param name="vector"> The math::RowVector being modified. </param>
    /// <param name="scaledDataVector"> The DataVector being added to the vector. </param>
    void operator+=(math::RowVectorReference<double> vector, const IDataVector& dataVector);

    /// <summary> Calculates the dot product of a DataVector and a math::ColumnVector. </summary>
    ///
    /// <param name="dataVector"> The DataVector. </param>
    /// <param name="vector"> The math::ColumnVector. </param>
    ///
    /// <returns> The result of the operation. </returns>
    double operator*(const IDataVector& dataVector, math::ColumnConstVectorReference<double> vector);
}
}
