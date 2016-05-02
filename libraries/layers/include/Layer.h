////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     Layer.h (layers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoordinateList.h"

// utilities
#include "XMLSerialization.h"
#include "StlIterator.h"
#include "TypeFactory.h"

// stl
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

namespace layers
{
    typedef utilities::StlIterator<std::vector<Coordinate>::const_iterator, Coordinate> InputCoordinateIterator;

    /// <summary> Implements a layer. </summary>
    class Layer
    {
    public:
        /// <summary> Default virtual destructor. </summary>
        virtual ~Layer() = default;

        /// <summary> Returns the number of elements in the layer. </summary>
        ///
        /// <returns> The number of elements in the layer. </returns>
        virtual uint64_t Size() const = 0;

        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="inputs"> The set of vectors holding input values to use </param>
        /// <param name="outputs"> [out] The vector where the output values are written. </param>
        virtual void Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const = 0;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> An iterator over the input coordinates. </returns>
        virtual CoordinateIterator GetInputCoordinateIterator(uint64_t index) const = 0;

        /// <summary> Returns the minimal required size of an input layer, which is the maximum input element from that layer plus 1. </summary>
        ///
        /// <param name="layerindex"> The layer index. </param>
        ///
        /// <returns> The required layer size. </returns>
        virtual uint64_t GetRequiredLayerSize(uint64_t layerIndex) const = 0;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Gets the name of the derived type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const = 0;

        /// <summary> Returns a TypeFactory that can construct subclasses of this type. </summary>
        ///
        /// <returns> a TypeFactory that can construct subclasses of this type. </returns> 
        static const utilities::TypeFactory<Layer> GetTypeFactory();

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


