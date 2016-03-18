////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Layer.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Coordinate.h"

// types
#include "types.h"
#include "StlIndexValueIterator.h"

// utilities
#include "XMLSerialization.h"
#include "StlIterator.h"

// stl
#include <vector>
#include <memory>
#include <string>

namespace layers
{
    /// <summary> Implements a layer. </summary>
    class Layer
    {
    public:
        /// <summary> Default virtual destructor. </summary>
        virtual ~Layer() = default;

        /// <summary> Returns the number of elements in the layer. </summary>
        ///
        /// <returns> The number of elements in the layer. </returns>
        virtual uint64 Size() const = 0;

        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="inputs"> The set of vectors holding input values to use </param>
        /// <param name="outputs"> [out] The vector where the output values are written. </param>
        virtual void Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const = 0;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> The input coordinates. </returns>
        using InputCoordinateIterator = utilities::StlIterator<std::vector<Coordinate>::const_iterator, Coordinate>;
        virtual InputCoordinateIterator GetInputCoordinates(uint64 index) const = 0;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static const char* GetTypeName();

        /// <summary> Gets the name of the derived type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual const char* GetRuntimeTypeName() const = 0;

        /// <summary> Reads the layer from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        virtual void Read(utilities::XMLDeserializer& deserializer) = 0;

        /// <summary> Writes the layer to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Write(utilities::XMLSerializer& serializer) const = 0;
    };
}


