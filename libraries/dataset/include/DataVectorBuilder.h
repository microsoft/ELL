// DataVectorBuilder.h

#pragma once

#include "IDataVector.h"

// stl
#include <memory>

namespace dataset
{
    /// DataVectorBuilder Base class
    ///
    template<typename DefaultDataVectorType>
    class DataVectorBuilder
    {
    public:

        /// Chooses the optimal DataVector representation for a list of index-value pairs
        ///
        template<typename IndexValueIteratorType>
        static std::unique_ptr<IDataVector> Build(IndexValueIteratorType IndexValueIterator);
    };
}

#include "../tcc/DataVectorBuilder.tcc"
