////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
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
    const int Sum::_currentVersion;

    Sum::Sum() 
    {}

    Sum::Sum(const CoordinateList & coordinates) : _inputCoordinates({ coordinates })
    {}

    Sum::Sum(const std::vector<CoordinateList>& coordinates) : _inputCoordinates(coordinates)
    {}

    uint64_t Sum::Size() const
    {
        return _inputCoordinates.size();
    }

    void Sum::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        for (uint64_t k = 0; k < _inputCoordinates.size(); ++k)
        {
            double output = 0;
            for (auto coordinate : _inputCoordinates[k])
            {
                output += inputs[coordinate.GetLayerIndex()][coordinate.GetElementIndex()];
            }
            outputs[k] = output;
        }
    }

    Layer::InputCoordinateIterator Sum::GetInputCoordinates(uint64_t index) const
    {
        return Layer::InputCoordinateIterator(_inputCoordinates[index].cbegin(), _inputCoordinates[index].cend());
    }

    std::string Sum::GetTypeName()
    {
        return "Sum";
    }

    std::string Sum::GetRuntimeTypeName() const
    {
        return GetTypeName();
    }

    void Sum::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("coordinates", _inputCoordinates);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

    void Sum::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}
