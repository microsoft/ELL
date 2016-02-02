// Layer.h
#pragma once

#include "Coordinate.h"

#include "types.h"
#include "RealArray.h"
using types::DoubleArray;

#include "VectorIterator.h"
using utilities::VectorIterator;

#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

#include <string>
using std::string;

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

        /// \returns A string tha trepresents the layer type
        ///
        string GetTypeName() const;

        /// Computes the output of the layer
        ///
        virtual void Compute(uint64 rowIndex, vector<DoubleArray>& outputs) const = 0; // TODO - resolve the naming row vs. layer

        /// \Returns An Iterator to the inputs that the specified output depends on
        ///
        virtual VectorIterator<Coordinate> GetInputCoordinates(uint64 index) const = 0;

        /// Serializes the Layer in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const = 0;
        
        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version) = 0;

    protected:
        void SerializeHeader(JsonSerializer& serializer, int version) const;

        Type _type;
        static const string typeName[];
    };
}


