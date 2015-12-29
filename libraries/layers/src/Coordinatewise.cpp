// Coordinatewise.cpp

#include "Coordinatewise.h"

#include <stdexcept>
using std::runtime_error;

#include <cassert>

#include <string>
using std::to_string;

namespace layers
{
    Coordinatewise::Coordinatewise(const DoubleOperation & operation, Type type) : Layer(type), _operation(operation)
    {}

    Coordinatewise::Coordinatewise(double value, Coordinate coordinate, const DoubleOperation & operation, Type type) : Layer(type), _operation(operation), _values(0), _coordinates(0)
    {
        _values.push_back(value);
        _coordinates.push_back(coordinate);
    }

    Coordinatewise::Coordinatewise(const vector<double> & values, const vector<Coordinate>& coordinates, const DoubleOperation& operation, Type type) : Layer(type), _operation(operation), _values(values), _coordinates(coordinates)
    {}

    uint64 Coordinatewise::Size() const
    {
        return _coordinates.size();
    }

    void Coordinatewise::Compute(uint64 rowIndex, vector<vector<double>>& outputs) const
    {
        for(uint64 k=0; k<_values.size(); ++k)
        {
            Coordinate coordinate = _coordinates[k];
            double input = outputs[coordinate.GetRow()][coordinate.GetColumn()];
            outputs[rowIndex][k] = _operation(_values[k], input);
        }
    }

    VectorIterator<Coordinate> Coordinatewise::GetInputCoordinates(uint64 index) const
    {
        return VectorIterator<Coordinate>(_coordinates.cbegin()+index, _coordinates.cbegin()+index+1);
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
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}


