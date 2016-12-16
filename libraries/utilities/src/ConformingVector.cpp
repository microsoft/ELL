////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConformingVector.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConformingVector.h"

namespace ell
{
namespace utilities
{
    BoolProxy::BoolProxy(bool value)
        : _value(value){};

    BoolProxy::BoolProxy(std::vector<bool>::reference value)
        : _value((bool)value) {}
}
}
