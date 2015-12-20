// Sum.cpp

#include "Sum.h"

#include <string>
using std::to_string;

namespace layers
{
    Sum::Sum() : Layer(1, Type::sum) 
    {}

    Sum::Sum(const vector<Coordinate> & coordinates) : Layer(1, Type::sum), _coordinates(0)
    {
        _coordinates.push_back(coordinates);
    }

    Sum::Sum(const vector<vector<Coordinate>>& coordinates) : Layer(coordinates.size(), Type::sum), _coordinates(coordinates)
    {}

    void Sum::Compute(const vector<unique_ptr<Layer>>& previousLayers)
    {
        for (uint64 k = 0; k<_coordinates.size(); ++k)
        {
            double output = 0;
            for (auto coordinate : _coordinates[k])
            {
                output += previousLayers[coordinate.GetRow()]->Get(coordinate.GetColumn());
            }
            _output[k] = output;
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
            _output.resize(1);
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }

}
