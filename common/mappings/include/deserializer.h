// deserializer.h

#pragma once

#define _USE_DEFAULT_DESERIALIZER_ void mappings::Deserialize(JsonSerializer& js, std::shared_ptr<Mapping>& up) {    mappings::DefaultDeserialize(js, up); }

#include "JsonSerializer.h"
#include "Mapping.h"

namespace mappings
{
    void Deserialize(JsonSerializer& js, std::shared_ptr<Mapping>& up);
    void DefaultDeserialize(JsonSerializer& js, std::shared_ptr<Mapping>& up);
}

