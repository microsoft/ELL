// Sum.cpp

#include "Sum.h"

// stl
#include <string>

#include <stdexcept>

namespace layers
{
    Sum::Sum() : Layer(Type::sum) 
    {}

    Sum::Sum(const CoordinateList & coordinates) : Layer(Type::sum), _coordinates(0)
    {
        _coordinates.push_back(coordinates);
    }

    Sum::Sum(const std::vector<CoordinateList>& coordinates) : Layer(Type::sum), _coordinates(coordinates)
    {}

    uint64 Sum::Size() const
    {
        return 1;
    }
    void Sum::Compute(uint64 rowIndex, std::vector<types::DoubleArray>& outputs) const
    {
        for (uint64 k = 0; k<_coordinates.size(); ++k)
        {
            double output = 0;
            for (auto coordinate : _coordinates[k])
            {
                output += outputs[coordinate.GetRow()][coordinate.GetColumn()];
            }
            outputs[rowIndex][k] = output;
        }
    }

    utilities::VectorIterator<Coordinate> Sum::GetInputCoordinates(uint64 index) const
    {
        return utilities::VectorIterator<Coordinate>(_coordinates[index].cbegin(), _coordinates[index].cend());
    }

    void Sum::Serialize(utilities::JsonSerializer & serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);
        serializer.Write("coordinates", _coordinates);
    }

    void Sum::Deserialize(utilities::JsonSerializer & serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("coordinates", _coordinates);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

}
