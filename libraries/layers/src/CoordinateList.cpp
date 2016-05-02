////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     CoordinateList.cpp (layers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CoordinateList.h"

namespace layers
{
    CoordinateList::CoordinateList(uint64_t layerIndex, uint64_t size) : _list(size)
    {
        for(uint64_t i=0; i<size; ++i)
        {
            _list[i] = Coordinate{layerIndex, i};
        }
    }

    void CoordinateList::AddCoordinate(const Coordinate & coordinate)
    {
        _list.push_back(coordinate);
    }

    uint64_t CoordinateList::Size() const
    {
        return _list.size();
    }

    const Coordinate & CoordinateList::operator[](uint64_t index) const
    {
        return _list[index];
    }

    void CoordinateList::AddCoordinate(uint64_t layerIndex, uint64_t elementIndex)
    {
        _list.emplace_back(layerIndex, elementIndex);
    }

    CoordinateIterator CoordinateList::GetIterator(uint64_t fromIndex, uint64_t size) const
    {
        uint64_t end = fromIndex+size;
        if(size == 0 || end > _list.size())
        {
            end = _list.size();
        }
        return CoordinateIterator(_list.begin()+fromIndex, _list.begin()+end);
    }

    uint64_t CoordinateList::GetRequiredLayerSize(uint64_t layerindex) const
    {
        uint64_t max = 0;
        for(auto coordinate : _list)
        {
            if(coordinate.GetLayerIndex() == layerindex && coordinate.GetElementIndex() >= max)
            {
                max = coordinate.GetElementIndex()+1;
            }
        }
        return max;
    }

    std::string CoordinateList::GetTypeName()
    {
        return "CoordinateList";
    }

    void CoordinateList::Read(utilities::XMLDeserializer& deserializer)
    {
        deserializer.Deserialize("list", _list);
    }

    void CoordinateList::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("list", _list);
    }

    void CoordinateList::Print(std::ostream & os) const
    {
        for(auto coordinate : _list)
        {
            os << coordinate << " ";
        }
    }

    std::ostream & operator<<(std::ostream & os, const CoordinateList& coordinateList)
    {
        coordinateList.Print(os);
        return os;
    }
}
