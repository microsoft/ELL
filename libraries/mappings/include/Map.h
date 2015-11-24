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
        /// Ctor
        ///
        Map(uint64 layerZeroSize);
    
        /// Computes the Map
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        void Compute(IndexValueIteratorType indexValueIterator);

    protected:
        vector<unique_ptr<Layer>> _layers;
    };
}

#include "../tcc/Map.tcc"

