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
#include <string>
#include <stdexcept>

namespace layers
{
    const int Input::_currentVersion;

    Input::Input(uint64 size) : _size(size)
    {}

    uint64 Input::Size() const
    {
        return _size;
    }

    void Input::Compute(uint64 layerIndex, std::vector<std::vector<double>>& outputs) const
    {
        throw std::runtime_error("this code should never be reached");
    }

    Layer::InputCoordinateIterator Input::GetInputCoordinates(uint64 index) const
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
    std::unique_ptr<Layer> Input::Clone() const
    {
        return std::make_unique<Input>(*this);
    }
}
