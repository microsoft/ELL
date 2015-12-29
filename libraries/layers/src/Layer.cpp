// Layer.cpp

#include "Layer.h"

namespace layers
{
    Layer::Layer(Type type) : _type(type)
    {}

    void Layer::SerializeHeader(JsonSerializer& serializer, int version) const
    {
        // to make the file more readable
        const string type_names[] = { "Input", "Scale", "Shift", "Sum", "DecisionTreePath" };
        serializer.Write("_type", type_names[(int)_type]);
        serializer.Write("_version", version);
    }
}