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
#include "StlIndexValueIteratorAdapter.h"

// utilities
#include "JsonSerializer.h"
#include "StlIteratorAdapter.h"

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
        enum class Type { zero, scale, shift, sum, decisionTreePath };

        /// <summary> Default ctor. </summary>
        ///
        /// <param name="type"> The type. </param>
        Layer(Type type);

        /// <summary> Default copy ctor. </summary>
        ///
        /// <param name="other"> The other. </param>
        Layer(const Layer& other) = default;

        /// <summary> Default move ctor. </summary>
        ///
        /// <param name="parameter1"> [in,out] The first parameter. </param>
        Layer(Layer&&) = default;

        /// <summary> Default virtual destructor. </summary>
        virtual ~Layer() = default;

        /// <summary> Returns the number of elements in the layer. </summary>
        ///
        /// <returns> The number of elements in the layer. </returns>
        virtual uint64 Size() const = 0;

        /// <summary> Gets a human friendly name of the layer type. </summary>
        ///
        /// <returns> The type name. </returns>
        std::string GetFriendlyLayerName() const;

        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="layerIndex"> The layer's own zero-based index in the map. </param>
        /// <param name="outputs"> [in,out] The vector where the output values are written. </param>
        virtual void Compute(uint64 layerIndex, std::vector<std::vector<double>>& outputs) const = 0;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> The input coordinates. </returns>
        using InputCoordinateIterator = utilities::StlIteratorAdapter<std::vector<Coordinate>::const_iterator, Coordinate>;
        virtual InputCoordinateIterator GetInputCoordinates(uint64 index) const = 0;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static const char* GetTypeName();

        /// <summary> Gets the name of the derived type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual const char* GetRuntimeTypeName() const = 0;

        // TODO: remove JSON serializaiton

        /// <summary> Serializes the Layer in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Serialize(utilities::JsonSerializer& serializer) const = 0;

        /// <summary> Deserializes the Layer in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        /// <param name="version"> The version. </param>
        virtual void Deserialize(utilities::JsonSerializer& serializer, int version) = 0;

    protected:
        void SerializeHeader(utilities::JsonSerializer& serializer, int version) const;

        Type _type;
        static const std::string typeName[];
    };
}


