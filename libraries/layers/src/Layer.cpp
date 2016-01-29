// Layer.cpp

#include "Layer.h"

namespace layers
{

    const string Layer::typeName[] = { "Input", "Scale", "Shift", "Sum", "DecisionTreePath" };

    Layer::Layer(Type type) : _type(type)
    {}

    string Layer::GetTypeName() const
    {
        return typeName[(int)_type];
    }

    void Layer::SerializeHeader(JsonSerializer& serializer, int version) const
    {
        // to make the file more readable
        serializer.Write("_type", GetTypeName());
        serializer.Write("_version", version);
    }
}