// Sum.cpp

#include "Sum.h"

#include <string>
using std::to_string;

namespace mappings
{
    Sum::Sum() : Layer(1, Type::sum) 
    {}

    Sum::Sum(const vector<Coordinate> & coordinates, double bias) : Layer(1, Type::sum), _coordinates(coordinates), _bias(bias)
    {}

    void Sum::Compute(const vector<unique_ptr<Layer>>& previousLayers)
    {
        double output = _bias;
        for (auto coordinate : _coordinates)
        {
            output += previousLayers[coordinate.GetRow()]->Get(coordinate.GetColumn());
        }
        _output[0] = output;
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
