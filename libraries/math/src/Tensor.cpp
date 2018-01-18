////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tensor.h"

// stl
#include <cassert>

namespace ell
{
namespace math
{
    //
    // TensorCoordinateBase
    //

    TensorCoordinateBase::TensorCoordinateBase(size_t rowValue, size_t columnValue, size_t channelValue) :
        _rowValue(rowValue), _columnValue(columnValue), _channelValue(channelValue)
    {}

    TensorCoordinateBase::TensorCoordinateBase(IntegerTriplet values) :
        _rowValue(values[0]), _columnValue(values[1]), _channelValue(values[2])
    {}

    TensorCoordinateBase::TensorCoordinateBase(const std::vector<size_t>& values) :
        _rowValue(values[0]), _columnValue(values[1]), _channelValue(values[2])
    {
        assert(values.size() == 3);
    }

    template<>
    size_t TensorCoordinateBase::GetValue<Dimension::row>() const 
    {
        return _rowValue;
    }

    template<>
    size_t TensorCoordinateBase::GetValue<Dimension::column>() const 
    {
        return _columnValue;
    }

    template<>
    size_t TensorCoordinateBase::GetValue<Dimension::channel>() const 
    {
        return _channelValue;
    }

    bool TensorCoordinateBase::operator==(const TensorCoordinateBase& other) const
    { 
        return _rowValue == other._rowValue && _columnValue == other._columnValue && _channelValue == other._channelValue;
    }
    
    bool TensorCoordinateBase::operator!=(const TensorCoordinateBase& other) const
    {
        return !((*this) == other);
    }
}
}