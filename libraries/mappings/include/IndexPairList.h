// IndexPairList.h

#pragma once

#include "IndexPair.h"

#include "types.h"

#include <vector>
using std::vector;

namespace mappings
{
    /// Represents an index value pair
    ///
    class IndexPairList 
    {
    public:

        /// Default Ctor
        ///
        IndexPairList();

        /// Copy ctor
        ///
        IndexPairList(const IndexPairList& other) = default;

        /// Move ctor
        ///
        IndexPairList(IndexPairList&&) = default;

        /// Adds an entry to the list
        ///
        void PushBack(uint64 i, uint64 j);

        /// Adds an interval of entries into the list
        ///
        void Fill(uint64 i, uint64 fromJ, uint64 numJ);

        /// \returns a specific entry in the list
        ///
        IndexPair Get(uint64 index) const;

        /// \returns the size of the list
        ///
        uint64 Size() const;

        /// Serializes an index value pair
        ///
        void Serialize(JsonSerializer& serializer) const;

        /// Deserializes an index value pair
        ///
        void Deserialize(JsonSerializer& serializer);

    private:
        vector<IndexPair> _list;
    };
}