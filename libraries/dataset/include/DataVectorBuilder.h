////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVectorBuilder.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// stl
#include <memory>

/// <summary> dataset namespace </summary>
namespace dataset
{
    /// <summary> DataVectorBuilder Base class. </summary>
    ///
    /// <typeparam name="DefaultDataVectorType"> Type of the default data vector type. </typeparam>
    template<typename DefaultDataVectorType>
    class DataVectorBuilder
    {
    public:

        /// <summary> Chooses the optimal DataVector representation for a list of index-value pairs. </summary>
        ///
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        ///
        /// <returns> A unique_ptr to an IDataVector</returns>
        template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> concept = 0>
        static std::unique_ptr<IDataVector> Build(IndexValueIteratorType IndexValueIterator);
    };
}

#include "../tcc/DataVectorBuilder.tcc"
