// Layer.h
#pragma once

#include "types.h"
#include "RealArray.h"
using common::DoubleArray;

#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

namespace layers
{
    class Layer
    {
    public:

        using Iterator = DoubleArray::Iterator;

        enum class Type { zero, scale, shift, sum, decisionTreePath };

        /// Default ctor
        ///
        Layer(uint64 size, Type type);

        /// Default copy ctor
        ///
        Layer(const Layer& other) = default;

        /// Default move ctor
        ///
        Layer(Layer&&) = default;

        /// Initializes the idenity layer by copying the input
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        void Set(IndexValueIteratorType indexValueIterator);

        /// Computes the output of the layer
        ///
        virtual void Compute(const vector<unique_ptr<Layer>>& previousLayers) = 0; 

        /// \returns The size of the layer's output
        ///
        uint64 Size() const;

        /// \returns The output value at a given index
        ///
        double Get(uint64 index) const;

        /// Sets the output to zero
        ///
        void Clear();

        /// \Returns An Iterator that points to the beginning of the vector.
        ///
        Iterator GetIterator() const;

        /// Serializes the Layer in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const = 0;
        
        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version) = 0;
                
    protected:
        void SerializeHeader(JsonSerializer& serializer, int version) const;

        Type _type;
        DoubleArray _output; // TODO make this vector<double> and move DoubleArray to linear library
    };
}

#include "../tcc/Layer.tcc"

