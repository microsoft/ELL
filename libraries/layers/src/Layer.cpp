// Layer.cpp

#include "Layer.h"

namespace layers
{

    const std::string Layer::typeName[] = { "Input", "Scale", "Shift", "Sum", "DecisionTreePath" };

    Layer::Layer(Type type) : _type(type)
    {}

    std::string Layer::GetTypeName() const
    {
        return typeName[(int)_type];
    }

    void Layer::SerializeHeader(utilities::JsonSerializer& serializer, int version) const
    {
        // to make the file more readable
        serializer.Write("_type", GetTypeName());
        serializer.Write("_version", version);
    }
}
