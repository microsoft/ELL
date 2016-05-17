////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CoordinateList.h (layers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Coordinate.h"

// utilities
#include "XMLSerialization.h"
#include "StlIterator.h"

// stl
#include <vector>
#include <iostream>

namespace layers
{
    typedef utilities::StlIterator<std::vector<Coordinate>::const_iterator, Coordinate> CoordinateIterator;

    /// <summary> Represents an element in a layer. </summary>
    class CoordinateList
    {
    public:

        CoordinateList() = default;

        /// <summary> Constructs a coordinateList for a specified number of elements in a given layer. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        /// <param name="size"> The number of elements. </param>
        CoordinateList(uint64_t layerIndex, uint64_t size);

        /// <summary> Adds a coordinate to the list. </summary>
        ///
        /// <param name="coordinate"> The coordinate to add. </param>
        void AddCoordinate(const Coordinate& coordinate);

        /// <summary> Gets the size. </summary>
        ///
        /// <returns> The number of coordinates in the list. </returns>
        uint64_t Size() const { return _list.size(); }

        /// <summary> Individual coordinate access operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        ///
        /// <returns> The indexed value. </returns>
        const Coordinate& operator[](uint64_t index) const;

        /// <summary> Adds a coordinate to the list. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        /// <param name="elementIndex"> Zero-based index of the element. </param>
        void AddCoordinate(uint64_t layerIndex, uint64_t elementIndex);

        /// <summary> Gets an iterator to the coordinates in the list </summary>
        ///
        /// <param name="fromIndex"> Zero-based index of the first coordinate. </param>
        /// <param name="size"> The number of coordinates to iterate over, or zero to continue until the end. </param>
        ///
        /// <returns> The coordinate iterator. </returns>
        CoordinateIterator GetIterator(uint64_t fromIndex=0, uint64_t size=0) const;
        
        /// <summary> Returns the minimal required size of a given layer, which is the maximum element associated with that layer plus 1. </summary>
        ///
        /// <param name="layerindex"> The layer index. </param>
        ///
        /// <returns> The required layer size. </returns>
        uint64_t GetRequiredLayerSize(uint64_t layerindex) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "CoordinateList"; }

        /// <summary> Reads the coordinate from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        void Read(utilities::XMLDeserializer& deserializer);

        /// <summary> Writes the coordinate to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        void Write(utilities::XMLSerializer& serializer) const;

        /// <summary> Prints the list to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Print(std::ostream& os) const;

    private:
        std::vector<Coordinate> _list;
    };

    /// <summary> Prints a Coordinate to a stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="coordinate"> The coordinate. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    std::ostream& operator<< (std::ostream& os, const CoordinateList& coordinateList);
}
