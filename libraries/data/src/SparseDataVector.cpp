////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SparseDataVector.cpp (data)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SparseDataVector.h"

namespace ell
{
namespace data
{
// float specialization
template<>
IDataVector::Type SparseDataVector<float, ell::utilities::CompressedIntegerList>::GetStaticType()
{
    return IDataVector::Type::SparseFloatDataVector;
}

// double specialization
template<>
IDataVector::Type SparseDataVector<double, ell::utilities::CompressedIntegerList>::GetStaticType()
{
    return IDataVector::Type::SparseDoubleDataVector;
}

// short specialization
template<>
IDataVector::Type SparseDataVector<short, ell::utilities::CompressedIntegerList>::GetStaticType()
{
    return IDataVector::Type::SparseShortDataVector;
}

// byte specialization
template<>
IDataVector::Type SparseDataVector<char, ell::utilities::CompressedIntegerList>::GetStaticType()
{
    return IDataVector::Type::SparseByteDataVector;
}
}
}