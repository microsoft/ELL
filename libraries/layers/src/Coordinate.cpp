////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Coordinate.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Coordinate.h"

namespace layers
{
    Coordinate::Coordinate(uint64 layerIndex, uint64 elementIndex) : _layerIndex(layerIndex), _elementIndex(elementIndex)
    {}

    uint64 Coordinate::GetLayerIndex() const
    {
        return _layerIndex;
    }

    uint64 Coordinate::GetElementIndex() const
    {
        return _elementIndex;
    }

    const char* Coordinate::GetTypeName()
    {
        return "Coordinate";
    }

    void Coordinate::Serialize(utilities::JsonSerializer& serializer) const
    {
      serializer.Write("l", (Json::UInt64)_layerIndex); 
      serializer.Write("e", (Json::UInt64)_elementIndex);
    }

    void Coordinate::Deserialize(utilities::JsonSerializer& serializer)
    {
        serializer.Read("l", _layerIndex);
        serializer.Read("e", _elementIndex);
    }

    std::ostream & operator<<(std::ostream & os, const Coordinate & coordinate)
    {
        os << '(' << coordinate.GetLayerIndex() << ',' << coordinate.GetElementIndex() << ')';
        return os;
    }
}
