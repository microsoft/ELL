////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Coordinatewise.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Coordinatewise.h"

// stl
#include <stdexcept>
#include <string>

namespace layers
{
    Coordinatewise::Coordinatewise(const DoubleOperation& operation, Type type) : Layer(type), _operation(operation)
    {}

    Coordinatewise::Coordinatewise(double value, Coordinate coordinate, const DoubleOperation & operation, Type type) : Layer(type), _values(0), _coordinates(0), _operation(operation)
    {
        _values.push_back(value);
        _coordinates.push_back(coordinate);
    }

    Coordinatewise::Coordinatewise(const std::vector<double> & values, const CoordinateList& coordinates, const DoubleOperation& operation, Type type) : Layer(type), _values(values), _coordinates(coordinates), _operation(operation)
    {}

    uint64 Coordinatewise::Size() const
    {
        return _coordinates.size();
    }

    void Coordinatewise::Compute(uint64 rowIndex, std::vector<types::DoubleArray>& outputs) const
    {
        for(uint64 k=0; k<_values.size(); ++k)
        {
            Coordinate coordinate = _coordinates[k];
            double input = outputs[coordinate.GetRow()][coordinate.GetColumn()];
            outputs[rowIndex][k] = _operation(_values[k], input);
        }
    }

    utilities::VectorIterator<Coordinate> Coordinatewise::GetInputCoordinates(uint64 index) const
    {
        return utilities::VectorIterator<Coordinate>(_coordinates.cbegin()+index, _coordinates.cbegin()+index+1);
    }

    void Coordinatewise::Serialize(utilities::JsonSerializer& serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);
        serializer.Write("values", _values);
        serializer.Write("coordinates", _coordinates);
    }

    void Coordinatewise::Deserialize(utilities::JsonSerializer& serializer, int version)
    {
        if(version == 1)
        {
            serializer.Read("values", _values);
            serializer.Read("coordinates", _coordinates);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }
}


