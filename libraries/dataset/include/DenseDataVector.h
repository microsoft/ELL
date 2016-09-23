////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// linear
#include "IVector.h"

// utilities
#include "StlIndexValueIterator.h"

// stl
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>
#include <initializer_list>

#define DEFAULT_DENSE_VECTOR_CAPACITY 1000

namespace emll
{
namespace dataset
{
    /// <summary> DenseDataVector Base class. </summary>
    ///
    /// <typeparam name="ValueType"> Type of the value type. </typeparam>
    template <typename ValueType>
    class DenseDataVector : public DataVectorBase<DenseDataVector<ValueType>>
    {
    public:
        using ConstIterator = utilities::VectorIndexValueIterator<ValueType>;

        /// <summary> Constructor. </summary>
        DenseDataVector();

        DenseDataVector(const DenseDataVector&) = default; // TODO look at ctors

        DenseDataVector(DenseDataVector&& other) = default;

        /// <summary> Constructs an instance of DenseDataVector. </summary>
        ///
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, typename concept = linear::IsIndexValueIterator<IndexValueIteratorType>>
        DenseDataVector(IndexValueIteratorType indexValueIterator);

        /// <summary> Constructs an instance of DenseDataVector from a std::vector. </summary>
        ///
        /// <param name="data"> The std::vector. </param>
        DenseDataVector(std::vector<ValueType> data);

        /// <summary> Constructs an instance of DenseDataVector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        DenseDataVector(std::initializer_list<linear::IndexValue> list);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the desired element. </param>
        ///
        /// <returns> Value of the desired element. </returns>
        double operator[](size_t index) const;

        /// <summary> Returns an Iterator that points to the beginning of the std::vector. </summary>
        ///
        /// <returns> The iterator. </returns>
        ConstIterator GetIterator() const { return utilities::MakeStlIndexValueIterator(_data); }

        /// <summary> Sets an entry in the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(size_t index, double value = 1.0) override;

       /// <summary> The largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> An size_t. </returns>
        virtual size_t Size() const override { return _data.size(); }

    private:
        size_t _numNonzeros;
        std::vector<ValueType> _data;
    };

    /// <summary> A float data vector. </summary>
    class FloatDataVector : public DenseDataVector<float>
    {
    public:
        using DenseDataVector<float>::DenseDataVector;
    };

    /// <summary> A double data vector. </summary>
    class DoubleDataVector : public DenseDataVector<double>
    {
    public:
        using DenseDataVector<double>::DenseDataVector;
    };
}
}

#include "../tcc/DenseDataVector.tcc"
