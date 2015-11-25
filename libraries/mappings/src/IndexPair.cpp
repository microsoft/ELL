// IndexPair.cpp

#include "IndexPair.h"

namespace mappings
{
    IndexPair::IndexPair(uint64 i, uint64 j) : _i(i), _j(j)
    {}

    uint64 IndexPair::GetI() const
    {
        return _i;
    }

    uint64 IndexPair::GetJ() const
    {
        return _j;
    }

    void IndexPair::Serialize(JsonSerializer& serializer) const
    {
        serializer.Write("i", _i);
        serializer.Write("j", _j);
    }

    void IndexPair::Deserialize(JsonSerializer& serializer)
    {
        serializer.Read("i", _i);
        serializer.Read("j", _j);
    }
}