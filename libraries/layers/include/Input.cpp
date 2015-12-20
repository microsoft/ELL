#include "Input.h"

#include <string>
using std::to_string;

#include<stdexcept>
using std::runtime_error;

namespace layers
{

    Input::Input(uint64 size) : Layer(size, Type::zero)
    {}

    void Input::Compute(const vector<unique_ptr<Layer>>& previousLayers)
    {
        Clear();
    }

    VectorIterator<Coordinate> Input::GetInputCoordinates(uint64 index) const
    {
        throw runtime_error("this place in the code should never be reached");
    }

    void Input::Serialize(JsonSerializer & serializer) const
    {
        // version 1
        Layer::SerializeHeader(serializer, 1);

        serializer.Write("size", _output.size());
    }

    void Input::Deserialize(JsonSerializer & serializer, int version)
    {
        if (version == 1)
        {
            uint64 size = 0;
            serializer.Read("size", size);
            _output.resize(size);
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}