// Coordinatewise.cpp

#include "Coordinatewise.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{
    Coordinatewise::Coordinatewise(const DoubleOperation & operation, Type type) : Layer(0, type), _operation(operation)
    {}

    Coordinatewise::Coordinatewise(const vector<double> & values, const IndexPairList & coordinates, const DoubleOperation& operation, Type type) : Layer(values.size(), type), _values(values), _coordinates(coordinates)
    {}

    void Coordinatewise::Compute(const vector<unique_ptr<Layer>>& previousLayers)
    {
        for(uint64 k=0; k<_values.size(); ++k)
        {
            IndexPair coordinate = _coordinates.Get(k);
            double input = previousLayers[coordinate.GetI()]->Get(coordinate.GetJ());
            _output[k] = _operation(_values[k], input);
        }
    }

    void Coordinatewise::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);

        serializer.Write("values", _values);
        serializer.Write("coordinates", _coordinates);

    }

    void Coordinatewise::Deserialize(JsonSerializer& serializer, int version)
    {
        if(version == 1)
        {
            serializer.Read("values", _values);
            serializer.Read("coordinates", _coordinates);
            _output.resize(_values.size());
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}


