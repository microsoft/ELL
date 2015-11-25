// IndexPairList.cpp

#include "IndexPairList.h"

namespace mappings
{
    IndexPairList::IndexPairList() : _list(0)
    {}

    void IndexPairList::PushBack(uint64 i, uint64 j)
    {
        _list.emplace_back(i,j);
    }

    void IndexPairList::Fill(uint64 i, uint64 fromJ, uint64 numJ)
    {
        for(uint64 j=fromJ; j < fromJ + numJ; ++j)
        {
            PushBack(i,j);
        }
    }

    IndexPair IndexPairList::Get(uint64 index) const
    {
        return _list[index];;
    }

    uint64 IndexPairList::Size() const
    {
        return _list.size();
    }

    void IndexPairList::Serialize(JsonSerializer& serializer) const
    {
        serializer.Write("list", _list);
    }

    void IndexPairList::Deserialize(JsonSerializer& serializer)
    {
        serializer.Read("list", _list);
    }
}