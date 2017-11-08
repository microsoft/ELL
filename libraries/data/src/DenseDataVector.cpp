////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DenseDataVector.cpp (data)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DenseDataVector.h"

namespace ell
{
namespace data
{
// float specialization
template<>
IDataVector::Type DenseDataVector<float>::GetStaticType()
{
    return IDataVector::Type::FloatDataVector;
}

// double specialization
template<>
IDataVector::Type DenseDataVector<double>::GetStaticType()
{
    return IDataVector::Type::DoubleDataVector;
}

// short specialization
template<>
IDataVector::Type DenseDataVector<short>::GetStaticType()
{
    return IDataVector::Type::ShortDataVector;
}

// byte specialization
template<>
IDataVector::Type DenseDataVector<char>::GetStaticType()
{
    return IDataVector::Type::ByteDataVector;
}
}
}