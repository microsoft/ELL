// IndexValue.cpp

#include "IndexValue.h"

namespace mappings
{
    IndexValue::IndexValue()
    {}

    IndexValue::IndexValue(int index, double value) : _index(index), _value(value)
    {}

    int IndexValue::GetIndex() const
    {
        return _index;
    }

    double IndexValue::GetValue() const
    {
        return _value;
    }

    void IndexValue::Serialize(JsonSerializer& serializer) const
    {
        serializer.Write("index", _index);
        serializer.Write("value", _value);
    }

    void IndexValue::Deserialize(JsonSerializer& serializer)
    {
        serializer.Read("index", _index);
        serializer.Read("value", _value);
    }
}