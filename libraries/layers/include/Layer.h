// Layer.h
#pragma once

#include "Coordinate.h"

#include "types.h"
#include "RealArray.h"

#include "VectorIterator.h"

#include "JsonSerializer.h"

#include <vector>

#include <memory>

#include <string>

namespace layers
{
    class Layer
    {
    public:
        enum class Type { zero, scale, shift, sum, decisionTreePath };

        /// Default ctor
        ///
        Layer(Type type);

        /// Default copy ctor
        ///
        Layer(const Layer& other) = default;

        /// Default move ctor
        ///
        Layer(Layer&&) = default;

        /// Default virtual destructor
        ///
        virtual ~Layer() = default;

        /// \returns The size of the layer's output
        ///
        virtual uint64 Size() const = 0;

        /// \returns A std::string tha trepresents the layer type
        ///
        std::string GetTypeName() const;

        /// Computes the output of the layer
        ///
        virtual void Compute(uint64 rowIndex, std::vector<types::DoubleArray>& outputs) const = 0; // TODO - resolve the naming row vs. layer

        /// \Returns An Iterator to the inputs that the specified output depends on
        ///
        virtual utilities::VectorIterator<Coordinate> GetInputCoordinates(uint64 index) const = 0;

        /// Serializes the Layer in json format
        ///
        virtual void Serialize(utilities::JsonSerializer& serializer) const = 0;
        
        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(utilities::JsonSerializer& serializer, int version) = 0;

    protected:
        void SerializeHeader(utilities::JsonSerializer& serializer, int version) const;

        Type _type;
        static const std::string typeName[];
    };
}


