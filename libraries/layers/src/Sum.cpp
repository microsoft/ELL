// Sum.cpp

#include "Sum.h"

// stl
#include <string>
using std::to_string;

#include <stdexcept>
using std::runtime_error;

namespace layers
{
    Sum::Sum() : Layer(Type::sum) 
    {}

    Sum::Sum(const CoordinateList & coordinates) : Layer(Type::sum), _coordinates(0)
    {
        _coordinates.push_back(coordinates);
    }

    Sum::Sum(const vector<CoordinateList>& coordinates) : Layer(Type::sum), _coordinates(coordinates)
    {}

    uint64 Sum::Size() const
    {
        return 1;
    }
    void Sum::Compute(uint64 rowIndex, vector<DoubleArray>& outputs) const
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

    VectorIterator<Coordinate> Sum::GetInputCoordinates(uint64 index) const
    {
        return VectorIterator<Coordinate>(_coordinates[index].cbegin(), _coordinates[index].cend());
    }

    void Sum::Serialize(JsonSerializer & serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);
        serializer.Write("coordinates", _coordinates);
    }

    void Sum::Deserialize(JsonSerializer & serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("coordinates", _coordinates);
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }

}
