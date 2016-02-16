////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Sum.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Sum.h"

// stl
#include <string>
#include <stdexcept>

namespace layers
{
    Sum::Sum() : Layer(Type::sum) 
    {}

    Sum::Sum(const CoordinateList & coordinates) : Layer(Type::sum), _inputCoordinates(0)
    {
        _inputCoordinates.push_back(coordinates);
    }

    Sum::Sum(const std::vector<CoordinateList>& coordinates) : Layer(Type::sum), _inputCoordinates(coordinates)
    {}

    uint64 Sum::Size() const
    {
        return 1;
    }
    void Sum::Compute(uint64 rowIndex, std::vector<types::DoubleArray>& outputs) const
    {
        for (uint64 k = 0; k<_inputCoordinates.size(); ++k)
        {
            double output = 0;
            for (auto coordinate : _inputCoordinates[k])
            {
                output += outputs[coordinate.GetLayerIndex()][coordinate.GetElementIndex()];
            }
            outputs[rowIndex][k] = output;
        }
    }

    utilities::VectorIterator<Coordinate> Sum::GetInputCoordinates(uint64 index) const
    {
        return utilities::VectorIterator<Coordinate>(_inputCoordinates[index].cbegin(), _inputCoordinates[index].cend());
    }

    void Sum::Serialize(utilities::JsonSerializer & serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);
        serializer.Write("coordinates", _inputCoordinates);
    }

    void Sum::Deserialize(utilities::JsonSerializer & serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("coordinates", _inputCoordinates);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

}
