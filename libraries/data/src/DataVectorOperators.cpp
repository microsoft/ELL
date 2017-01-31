////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVectorOperators.cpp (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVectorOperators.h"

namespace ell
{
namespace data
{
    ScaledDataVector::ScaledDataVector(double scalar, const IDataVector& dataVector)
        : _scalar(scalar), _dataVector(dataVector)
    {
    }

    ScaledDataVector operator*(double scalar, const IDataVector& dataVector)
    {
        return ScaledDataVector(scalar, dataVector);
    }

    void operator+=(math::RowVectorReference<double> vector, const ScaledDataVector& scaledDataVector)
    {
        scaledDataVector.GetDataVector().AddTo(vector, scaledDataVector.GetScalar());
    }

    void operator+=(math::RowVectorReference<double> vector, const IDataVector& dataVector)
    {
        dataVector.AddTo(vector);
    }

    double operator*(const IDataVector& dataVector, math::ColumnConstVectorReference<double> vector)
    {
        return dataVector.Dot(vector);
    }
}
}