#include "Input.h"

// stl
#include <string>
#include <stdexcept>

namespace layers
{

    Input::Input(uint64 size) : Layer(Type::zero), _size(size)
    {}

    uint64 Input::Size() const
    {
        return _size;
    }

    void Input::Compute(uint64 rowIndex, vector<types::DoubleArray>& outputs) const
    {
        throw std::runtime_error("this code should never be reached");
    }

    utilities::VectorIterator<Coordinate> Input::GetInputCoordinates(uint64 index) const
    {
        throw std::runtime_error("this code should never be reached");
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
