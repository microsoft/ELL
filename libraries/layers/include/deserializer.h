// deserializer.h

#pragma once

// IMPORTANT: Any project that uses layers must specify a deserializer. Most projects will just use this macro
//
#define _USE_DEFAULT_DESERIALIZER_ void layers::Deserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up) { layers::DefaultDeserialize(serializer, up); }

#include "Layer.h"
#include "Map.h"

// utilities
#include "JsonSerializer.h"
using utilities::JsonSerializer;

namespace layers
{
    void Deserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up);

    void Deserialize(JsonSerializer& serializer, std::shared_ptr<Map>& up);

    void DefaultDeserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up);
}

