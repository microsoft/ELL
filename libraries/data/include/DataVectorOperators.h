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
    class ScaledDataVector
    {
    public:
        // TODO document
        ScaledDataVector(double scalar, const IDataVector& dataVector);

        // TODO document
        double GetScalar() const { return _scalar; }

        // TODO document
        const IDataVector& GetDataVector() const { return _dataVector; }

    private:
        double _scalar;
        const IDataVector& _dataVector;
    };

    // TODO document
    ScaledDataVector operator*(double scalar, const IDataVector& dataVector);

    // TODO document
    void operator+=(math::RowVectorReference<double> vector, const ScaledDataVector& scaledDataVector);

    // TODO document
    void operator+=(math::RowVectorReference<double> vector, const IDataVector& dataVector);

    // TODO document
    double operator*(const IDataVector& dataVector, math::UnorientedConstVectorReference<double> vector);
}
}
