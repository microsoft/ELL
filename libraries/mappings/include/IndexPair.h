// IndexPair.h

#pragma once

// utilities
#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include "types.h"

namespace mappings
{
    /// Represents an index value pair
    ///
    class IndexPair
    {
    public:

        /// Ctor
        ///
        IndexPair(uint64 i=0, uint64 j=0);

        /// Copy ctor
        ///
        IndexPair(const IndexPair&) = default;

        /// Move ctor
        ///
        IndexPair(IndexPair&&) = default;

        /// \returns the i index
        ///
        uint64 GetI() const;

        /// \returns the j index
        ///
        uint64 GetJ() const; 

        /// Serializes an index value pair
        ///
        void Serialize(JsonSerializer& serializer) const;

        /// Deserializes an index value pair
        ///
        void Deserialize(JsonSerializer& serializer);

    private:
        uint64 _i = 0;
        uint64 _j = 0;
    };
}