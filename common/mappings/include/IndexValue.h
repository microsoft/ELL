// IndexValue.h

#pragma once

#include "Mapping.h"

namespace mappings
{
    /// Represents an index value pair
    ///
    class IndexValue
    {
    public:

        IndexValue();

        /// Constructs an index value
        ///
        IndexValue(int index, double value);

        /// \return The index
        ///
        int GetIndex() const;

        /// \returns The value
        ///
        double GetValue() const;

        /// Serializes an index value pair
        ///
        void Serialize(JsonSerializer& js) const;

        /// Deserializes an index value pair
        ///
        void Deserialize(JsonSerializer& js);

    private:
        int _index;
        double _value;
    };
}