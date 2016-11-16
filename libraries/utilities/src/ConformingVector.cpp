////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConformingVector.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConformingVector.h"

namespace emll
{
namespace utilities
{
    BoolProxy::BoolProxy(bool value)
        : _value(value){};
    
    BoolProxy::BoolProxy(std::vector<bool>::reference value)
        : _value((bool)value) {}
}
}
