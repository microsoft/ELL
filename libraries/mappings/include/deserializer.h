// deserializer.h

#pragma once

// IMPORTANT: Any project that uses layers must specify a deserializer. Most projects will just use this macro
//
#define _USE_DEFAULT_DESERIALIZER_ void mappings::Deserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up) { mappings::DefaultDeserialize(serializer, up); }

#include "Layer.h"
#include "Map.h"

// utilities
#include "JsonSerializer.h"
using utilities::JsonSerializer;

namespace mappings
{
    void Deserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up);

    void Deserialize(JsonSerializer& serializer, std::shared_ptr<Map>& up);

    void DefaultDeserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up);
}

