// Map.h

#pragma once

#include "Layer.h"

#include "types.h"

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

namespace mappings
{
    class Map
    {

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

    protected:
        vector<unique_ptr<Layer>> _layers;
    };
}

#include "../tcc/Map.tcc"

