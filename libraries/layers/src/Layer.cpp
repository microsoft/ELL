// Layer.cpp

#include "Layer.h"

namespace layers
{
    Layer::Layer(uint64 size, Type type) : _output(size), _type(type)
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
        const string type_names[] = { "Zero", "Scale", "Shift", "Sum", "DecisionTreePath" };
        serializer.Write("_type", type_names[(int)_type]);
        serializer.Write("_version", version);
    }
}