// Map.h

#pragma once

#include "Layer.h"
#include "Coordinate.h"

// common
#include "types.h"

// predictors
#include "SharedLinearBinaryPredictor.h"
using predictors::SharedLinearBinaryPredictor;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

namespace mappings
{
    class Map
    {
    public:
        using Iterator = DoubleArray::Iterator;

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

        /// Adds a shared linear predictor to the map, as a scale layer followed by a sum layer
        ///
        void Add(const SharedLinearBinaryPredictor& predictor, const vector<Coordinate>& predictorInput);

        /// Serializes the Map in json format
        ///
        void Serialize(JsonSerializer& serializer) const;

        /// Deserializes the Map in json format
        ///
        void Deserialize(JsonSerializer& serializer);

    protected:
        vector<shared_ptr<Layer>> _layers;
    };
}

#include "../tcc/Map.tcc"

