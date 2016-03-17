////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Input.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Input.h"

// stl
#include <cstdint>
#include <string>
#include <stdexcept>
#include <algorithm>

namespace layers
{
    const int Input::_currentVersion;

    Input::Input() : _size(0)
    {}

    uint64_t Input::Size() const
    {
        return _size;
    }

    void Input::IncreaseSize(uint64_t minimumSize)
    {
        _size = std::max(_size, minimumSize);
    }

    void Input::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        throw std::runtime_error("this code should never be reached");
    }

    Layer::InputCoordinateIterator Input::GetInputCoordinates(uint64_t index) const
    {
        throw std::runtime_error("this code should never be reached");
    }

    std::string Input::GetTypeName()
    {
        return "Input";
    }

    std::string Input::GetRuntimeTypeName() const
    {
        return GetTypeName();
    }

    void Input::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("size", _size);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

    void Input::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("size", _size);
    }
}
