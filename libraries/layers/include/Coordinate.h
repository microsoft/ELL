////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Coordinate.h (layers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "XMLSerialization.h"

// stl
#include <cstdint>
#include <vector>
#include <iosfwd>

namespace layers
{
    /// <summary> Represents an element in a layer. </summary>
    class Coordinate 
    {
    public:

        /// <summary> Default constructor. Sets layer and element indices to 0. </summary>
        Coordinate();

        /// <summary> Constructs a Coordinate object. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        /// <param name="elementIndex"> Zero-based index of the element. </param>
        Coordinate(uint64_t layerIndex, uint64_t elementIndex);

        /// <summary> Gets the layer index. </summary>
        ///
        /// <returns> The layer index. </returns>
        uint64_t GetLayerIndex() const;

        /// <summary> Gets the element index. </summary>
        ///
        /// <returns> The element index. </returns>
        uint64_t GetElementIndex() const; 

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Reads the coordinate from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        void Read(utilities::XMLDeserializer& deserializer);

        /// <summary> Writes the coordinate to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        void Write(utilities::XMLSerializer& serializer) const;

    private:
        uint64_t _layerIndex = 0;
        uint64_t _elementIndex = 0;
    };

    /// <summary> Prints a Coordinate to a stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="coordinate"> The coordinate. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    std::ostream& operator<< (std::ostream& os, const Coordinate& coordinate);
}
