// deserializer.h

#pragma once

#define _USE_DEFAULT_DESERIALIZER_ void mappings::Deserialize(JsonSerializer& serializer, std::shared_ptr<Mapping>& up) {    mappings::DefaultDeserialize(serializer, up); }

#include "JsonSerializer.h"
#include "Mapping.h"

namespace mappings
{
    void Deserialize(JsonSerializer& serializer, std::shared_ptr<Mapping>& up);

    void DefaultDeserialize(JsonSerializer& serializer, std::shared_ptr<Mapping>& up);
}

