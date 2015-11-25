// Layer.cpp

#include "Layer.h"

namespace mappings
{
    Layer::Layer(uint64 size) : _output(size)
    {}

    void Layer::Compute(const vector<unique_ptr<Layer>>& previousLayers)
    {}

    uint64 Layer::Size() const
    {
        return _output.Size();
    }

    double Layer::Get(uint64 index) const
    {
        return _output[index];
    }

    void Layer::Clear()
    {
        std::fill(_output.begin(), _output.end(), 0);
    }

    Layer::Iterator Layer::GetIterator() const
    {
        return _output.GetIterator();
    }

    void Layer::SerializeHeader(JsonSerializer & serializer, int version) const
    {
        // to make the file more readable
        const string type_names[] = {"constant", "scale", "shift", "sum", "decisionTreePath", "row", "column", "null"};
        serializer.Write("_type", type_names[(int)_type]);
        serializer.Write("_version", version);
    }
}