////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataVectorOperations.cpp (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVectorOperations.h"

namespace ell
{
namespace data
{
    double operator*(math::UnorientedConstVectorReference<double> vector, const IDataVector& dataVector)
    {
        return dataVector.Dot(vector);
    }

    double operator*(const IDataVector& dataVector, math::UnorientedConstVectorReference<double> vector)
    {
        return vector * dataVector;
    }
}
}