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
#include "XMLSerialization.h"

// stl
#include <vector>
#include <iostream>

namespace layers
{
    /// <summary> Represents an element in a layer. </summary>
    class Coordinate final
    {
    public:

        /// <summary> Constructs a Coordinate object. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        /// <param name="elementIndex"> Zero-based index of the element. </param>
        Coordinate(uint64 layerIndex=0, uint64 elementIndex=0);

        /// <summary> Gets the layer index. </summary>
        ///
        /// <returns> The layer index. </returns>
        uint64 GetLayerIndex() const;

        /// <summary> Gets the element index. </summary>
        ///
        /// <returns> The element index. </returns>
        uint64 GetElementIndex() const; 

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Reads the map from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        void Read(utilities::XMLDeserializer& deserializer);

        /// <summary> Writes the map to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        void Write(utilities::XMLSerializer& serializer) const;

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
