// Coordinate.h

#pragma once

// utilities
#include "JsonSerializer.h"

#include "types.h"

// stl
#include <vector>

namespace layers
{
    /// Represents an index value pair
    ///
    class Coordinate final
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
        void Serialize(utilities::JsonSerializer& serializer) const;

        /// Deserializes an index value pair
        ///
        void Deserialize(utilities::JsonSerializer& serializer);

    private:
        uint64 _row = 0;
        uint64 _column = 0;
    };

    using  CoordinateList = std::vector<Coordinate>;
}
