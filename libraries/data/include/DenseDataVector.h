////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVector.h"

#ifndef DENSEDATAVECTOR_H
#define DENSEDATAVECTOR_H

// utilities
#include "StlIndexValueIterator.h"

// stl
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>
#include <initializer_list>

#define DEFAULT_DENSE_VECTOR_CAPACITY 256

namespace emll
{
namespace data
{
    /// <summary> DenseDataVector Base class. </summary>
    ///
    /// <typeparam name="ElementType"> Type of the value type. </typeparam>
    template <typename ElementType>
    class DenseDataVector : public DataVectorBase<DenseDataVector<ElementType>>
    {
    public:
        using Iterator = VectorIndexValueIterator<ElementType>; 

        /// <summary> Constructor. </summary>
        DenseDataVector();

        DenseDataVector(DenseDataVector&& other) = default;

        DenseDataVector(const DenseDataVector&) = delete;

        /// <summary> Constructs a DenseDataVector from an index value iterator. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept = true>
        DenseDataVector(IndexValueIteratorType indexValueIterator);

        /// <summary> Constructs a data vector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        DenseDataVector(std::initializer_list<IndexValue> list);

        /// <summary> Constructs a data vector from an initializer list of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        DenseDataVector(std::initializer_list<double> list);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the desired element. </param>
        ///
        /// <returns> Value of the desired element. </returns>
        double operator[](size_t index) const;

        /// <summary> Returns an Iterator that points to the beginning of the std::vector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const { return Iterator(_data.begin(), _data.end()); }

        /// <summary> Appends an element to the end of the data vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the element, must be bigger than the biggest current index. </param>
        /// <param name="value"> The element value. </param>
        virtual void AppendElement(size_t index, double value) override;

        /// <summary>
        /// A data vector has infinite dimension and ends with a suffix of zeros. This function returns
        /// the first index in this suffix. Equivalently, the returned value is one plus the index of the
        /// last non-zero element.
        /// </summary>
        ///
        /// <returns> The first index of the suffix of zeros at the end of this vector. </returns>
        virtual size_t PrefixLength() const override { return _data.size(); }

    private:
        using DataVectorBase<DenseDataVector<ElementType>>::AppendElements;
        size_t _numNonzeros = 0;
        std::vector<ElementType> _data;
    };

    /// <summary> A dense data vector with double elements. </summary>
    struct DoubleDataVector : public DenseDataVector<double>
    {
        using DenseDataVector<double>::DenseDataVector;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::DoubleDataVector; }
    };

    /// <summary> A dense data vector with float elements. </summary>
    struct FloatDataVector : public DenseDataVector<float>
    {
        using DenseDataVector<float>::DenseDataVector;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::FloatDataVector; }
    };

    /// <summary> A dense data vector with short elements. </summary>
    struct ShortDataVector : public DenseDataVector<short>
    {
        using DenseDataVector<short>::DenseDataVector;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::ShortDataVector; }
    };

    /// <summary> A dense data vector with byte elements. </summary>
    struct ByteDataVector : public DenseDataVector<char>
    {
        using DenseDataVector<char>::DenseDataVector;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::ByteDataVector; }
    };
}
}

#include "../tcc/DenseDataVector.tcc"

#endif // DENSEDATAVECTOR_H
