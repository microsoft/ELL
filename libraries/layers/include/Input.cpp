#include "Input.h"

#include <string>
using std::to_string;

#include<stdexcept>
using std::runtime_error;

namespace layers
{

    Input::Input(uint64 size) : Layer(Type::zero), _size(size)
    {}

    uint64 Input::Size() const
    {
        return _size;
    }

    void Input::Compute(uint64 rowIndex, vector<DoubleArray>& outputs) const
    {
        throw runtime_error("this code should never be reached");
    }

    VectorIterator<Coordinate> Input::GetInputCoordinates(uint64 index) const
    {
        throw runtime_error("this code should never be reached");
    }

    void Input::Serialize(JsonSerializer & serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);

        serializer.Write("size", _size);
    }

    void Input::Deserialize(JsonSerializer & serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("size", _size);
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}