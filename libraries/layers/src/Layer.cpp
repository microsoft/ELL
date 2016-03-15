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
    const char* Layer::GetTypeName()
    {
        return "Layer";
    }

    void Layer::SerializeHeader(utilities::JsonSerializer& serializer, int version) const // TODO
    {
        // to make the file more readable
        //serializer.Write("_type", GetFriendlyLayerName());
        //serializer.Write("_version", version);
    }
}
