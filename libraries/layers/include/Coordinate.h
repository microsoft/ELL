////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Coordinate.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// utilities
#include "JsonSerializer.h"

// stl
#include <vector>
#include <iostream>

namespace layers
{
    /// Represents an index value pair
    ///
    class Coordinate final
    {
    public:

        /// Ctor
        ///
        Coordinate(uint64 layerIndex=0, uint64 elementIndex=0);

        /// Copy ctor
        ///
        Coordinate(const Coordinate&) = default;

        /// Move ctor
        ///
        Coordinate(Coordinate&&) = default;

        /// \returns the i index
        ///
        uint64 GetLayerIndex() const;

        /// \returns the j index
        ///
        uint64 GetElementIndex() const; 

        /// Serializes an index value pair
        ///
        void Serialize(utilities::JsonSerializer& serializer) const;

        /// Deserializes an index value pair
        ///
        void Deserialize(utilities::JsonSerializer& serializer);

    private:
        uint64 _layerIndex = 0;
        uint64 _elementIndex = 0;
    };

    /// <summary> Prints a Coordinate to a stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="coordinate"> The coordinate. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    std::ostream& operator<< (std::ostream& os, const Coordinate& coordinate);

    typedef std::vector<Coordinate> CoordinateList;
}
