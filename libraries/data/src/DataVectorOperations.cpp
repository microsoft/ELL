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

    double operator*(const IDataVector& dataVector, math::UnorientedConstVectorBase<double> vector)
    {
        return vector * dataVector;
    }
}
}