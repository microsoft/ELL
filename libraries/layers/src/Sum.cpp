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
    const int Sum::_currentVersion;

    Sum::Sum() 
    {}

    Sum::Sum(const CoordinateList & coordinates) : _inputCoordinates(0)
    {
        _inputCoordinates.push_back(coordinates);
    }

    Sum::Sum(const std::vector<CoordinateList>& coordinates) : _inputCoordinates(coordinates)
    {}

    uint64 Sum::Size() const
    {
        return 1;
    }
    void Sum::Compute(uint64 layerIndex, std::vector<std::vector<double>>& outputs) const
    {
        for (uint64 k = 0; k<_inputCoordinates.size(); ++k)
        {
            double output = 0;
            for (auto coordinate : _inputCoordinates[k])
            {
                output += outputs[coordinate.GetLayerIndex()][coordinate.GetElementIndex()];
            }
            outputs[layerIndex][k] = output;
        }
    }

    Layer::InputCoordinateIterator Sum::GetInputCoordinates(uint64 index) const
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
