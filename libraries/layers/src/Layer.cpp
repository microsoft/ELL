////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Layer.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Layer.h"

namespace layers
{
    const std::string Layer::typeName[] = { "Input", "Scale", "Shift", "Sum", "DecisionTreePath" };

    Layer::Layer(Type type) : _type(type)
    {}

    std::string Layer::GetFriendlyLayerName() const
    {
        return typeName[(int)_type];
    }

    const char* Layer::GetTypeName()
    {
        return "Layer";
    }

    void Layer::SerializeHeader(utilities::JsonSerializer& serializer, int version) const
    {
        // to make the file more readable
        serializer.Write("_type", GetFriendlyLayerName());
        serializer.Write("_version", version);
    }
}
