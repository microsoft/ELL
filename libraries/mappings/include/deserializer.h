// deserializer.h

#pragma once

#define _USE_DEFAULT_DESERIALIZER_ void mappings::Deserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up) { mappings::DefaultDeserialize(serializer, up); }

#include "Layer.h"

// utilities
#include "JsonSerializer.h"
using utilities::JsonSerializer;

namespace mappings
{
    void Deserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up);

    void DefaultDeserialize(JsonSerializer& serializer, std::shared_ptr<Layer>& up);
}

