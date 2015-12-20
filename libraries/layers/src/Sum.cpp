// Sum.cpp

#include "Sum.h"

#include <string>
using std::to_string;

namespace layers
{
    Sum::Sum() : Layer(1, Type::sum) 
    {}

    Sum::Sum(double bias, const vector<Coordinate> & coordinates) : Layer(1, Type::sum), _bias(bias), _coordinates(coordinates)
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

    VectorIterator<Coordinate> Sum::GetInputCoordinates(uint64 index) const
    {
        if (index != 0)
        {
            throw runtime_error("sum only has one output");
        }

        return VectorIterator<Coordinate>(_coordinates.cbegin(), _coordinates.cend());
    }

    void Sum::Serialize(JsonSerializer & serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);
        serializer.Write("bias", _bias);
        serializer.Write("coordinates", _coordinates);
    }

    void Sum::Deserialize(JsonSerializer & serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("bias", _bias);
            serializer.Read("coordinates", _coordinates);
            _output.resize(1);
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }

}
