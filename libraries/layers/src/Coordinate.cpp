////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Coordinate.cpp (layers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Coordinate.h"

namespace layers
{
    Coordinate::Coordinate() : _layerIndex(0), _elementIndex(0)
    {}

    Coordinate::Coordinate(uint64_t layerIndex, uint64_t elementIndex) : _layerIndex(layerIndex), _elementIndex(elementIndex)
    {}

    uint64_t Coordinate::GetLayerIndex() const
    {
        return _layerIndex;
    }

    uint64_t Coordinate::GetElementIndex() const
    {
        return _elementIndex;
    }

    std::string Coordinate::GetTypeName()
    {
        return "Coordinate";
    }

    void Coordinate::Read(utilities::XMLDeserializer& deserializer)
    {
        deserializer.Deserialize("l", _layerIndex);
        deserializer.Deserialize("e", _elementIndex);
    }

    void Coordinate::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("l", _layerIndex);
        serializer.Serialize("e", _elementIndex);
    }

    std::ostream & operator<<(std::ostream & os, const Coordinate & coordinate)
    {
        os << '(' << coordinate.GetLayerIndex() << ',' << coordinate.GetElementIndex() << ')';
        return os;
    }
}
