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

    Input::Input() : Layer(Type::zero), _size(0)
    {}

    uint64 Input::Size() const
    {
        return _size;
    }

    void Input::SetSize(uint64 size)
    {
        _size = size;
    }

    void Input::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        throw std::runtime_error("this code should never be reached");
    }

    Layer::InputCoordinateIterator Input::GetInputCoordinates(uint64 index) const
    {
        throw std::runtime_error("this code should never be reached");
    }

    const char* Input::GetTypeName()
    {
        return "Input";
    }

    const char* Input::GetRuntimeTypeName() const
    {
        return GetTypeName();
    }

    void Input::Read(utilities::XMLDeserializer& deserializer)
    {
        //int version = 0;
        //deserializer.Deserialize("version", version);
        //if (version == 1)
        //{
        //    deserializer.Deserialize("size", _size);
        //}
        //else
        //{
        //    throw std::runtime_error("unsupported version: " + std::to_string(version));
        //}
    }

    void Input::Write(utilities::XMLSerializer& serializer) const
    {
        //serializer.Serialize("version", _currentVersion);
        //serializer.Serialize("size", _size);
    }

    void Input::Serialize(utilities::JsonSerializer & serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);

        serializer.Write("size", (Json::UInt64)_size);
    }

    void Input::Deserialize(utilities::JsonSerializer & serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("size", _size);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }
}
