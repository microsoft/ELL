// Coordinate.h

#pragma once

// utilities
#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include "types.h"

namespace mappings
{
    /// Represents an index value pair
    ///
    class Coordinate
    {
    public:

        /// Ctor
        ///
        Coordinate(uint64 row=0, uint64 column=0);

        /// Copy ctor
        ///
        Coordinate(const Coordinate&) = default;

        /// Move ctor
        ///
        Coordinate(Coordinate&&) = default;

        /// \returns the i index
        ///
        uint64 GetRow() const;

        /// \returns the j index
        ///
        uint64 GetColumn() const; 

        /// Serializes an index value pair
        ///
        void Serialize(JsonSerializer& serializer) const;

        /// Deserializes an index value pair
        ///
        void Deserialize(JsonSerializer& serializer);

        static void FillBack(vector<Coordinate>& vector, uint64 row, uint64 numColumns, uint64 fromColumn=0);

    private:
        uint64 _row = 0;
        uint64 _column = 0;
    };
}