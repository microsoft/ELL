// SerializableIndexValue.cpp

#include "SerializableIndexValue.h"

namespace mappings
{
    void SerializableIndexValue::Serialize(JsonSerializer& serializer) const
    {
        serializer.Write("index", index);
        serializer.Write("value", value);
    }

    void SerializableIndexValue::Deserialize(JsonSerializer& serializer)
    {
        serializer.Read("index", index);
        serializer.Read("value", value);
    }
}