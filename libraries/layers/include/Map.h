// Map.h

#pragma once

#include "Layer.h"
#include "Coordinate.h"

// common
#include "types.h"

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include <iostream>
using std::istream;

namespace layers
{
    class Map
    {
    public:
        using Iterator = DoubleArray::Iterator;

        Map() = default;
        
        /// Ctor
        ///
        Map(uint64 inputLayerSize);
    
        /// Computes the Map
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        void Compute(IndexValueIteratorType indexValueIterator);

        /// \Returns An Iterator that points to the beginning of a specified layer's output.
        ///
        Iterator GetIterator(uint64 layerIndex = maxUInt64) const;

        /// Adds a shared layer to the map
        /// \returns The row index of the added layer
        uint64 PushBack(shared_ptr<Layer> layer);

        /// Serializes the Map in json format
        ///
        void Serialize(JsonSerializer& serializer) const;

        /// Serializes the Map in json format
        ///
        void Serialize(ostream& os) const;

        /// Deserializes the Map in json format
        ///
        void Deserialize(JsonSerializer& serializer);

        /// reads a Map from a stream
        ///
        template<typename MapType = Map>
        static shared_ptr<MapType> Deserialize(istream& is);

        /// Static function for deserializing shared_ptr<Layer>
        ///
        static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<Layer>& up);

    protected:
        vector<shared_ptr<Layer>> _layers;
    };
}

#include "../tcc/Map.tcc"

