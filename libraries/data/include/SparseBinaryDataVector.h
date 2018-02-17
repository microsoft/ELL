////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SparseBinaryDataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVector.h"
#include "IndexValue.h"

#ifndef SPARSEBINARYDATAVECTOR_H
#define SPARSEBINARYDATAVECTOR_H

// utilities
#include "CompressedIntegerList.h"
#include "IntegerList.h"

// stl
#include <cstddef>
#include <initializer_list>
#include <vector>

namespace ell
{
namespace data
{
    // forward declaration
    template <typename IndexListType>
    class SparseBinaryDataVectorBase;

    // forward declaration
    template <IterationPolicy policy, typename IndexListType>
    class SparseBinaryDataVectorIterator;

    /// <summary> A dense iterator for SparseBinaryDataVectors. </summary>
    template <typename IndexListType>
    class SparseBinaryDataVectorIterator<IterationPolicy::all, IndexListType> : public IIndexValueIterator
    {
    public:
        SparseBinaryDataVectorIterator(const SparseBinaryDataVectorIterator<IterationPolicy::all, IndexListType>&) = default;

        SparseBinaryDataVectorIterator(SparseBinaryDataVectorIterator<IterationPolicy::all, IndexListType>&&) = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const { return _index < _size; }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the current iterate. </summary>
        ///
        /// <returns> The current iterate. </returns>
        IndexValue Get() const;

    protected:

        using IndexIteratorType = typename IndexListType::Iterator;

        // private ctor that can only be called from the parent class
        SparseBinaryDataVectorIterator(const IndexIteratorType& listIterator, size_t size);
        friend SparseBinaryDataVectorBase<IndexListType>;

        IndexIteratorType _indexIterator;
        size_t _iteratorIndex = 0;
        size_t _size = 0;
        size_t _index = 0;
    };

    /// <summary> A read-only forward iterator for the sparse binary vector. </summary>
    template <typename IndexListType>
    class SparseBinaryDataVectorIterator<IterationPolicy::skipZeros, IndexListType> : public IIndexValueIterator
    {
    public:
        SparseBinaryDataVectorIterator(const SparseBinaryDataVectorIterator<IterationPolicy::skipZeros, IndexListType>&) = default;

        SparseBinaryDataVectorIterator(SparseBinaryDataVectorIterator<IterationPolicy::skipZeros, IndexListType>&&) = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const { return _indexIterator.IsValid() && _indexIterator.Get() < _size; }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next() { _indexIterator.Next(); }

        /// <summary> Returns The current value. </summary>
        ///
        /// <returns> An IndexValue. </returns>
        IndexValue Get() const { return IndexValue{ _indexIterator.Get(), 1.0 }; }

    private:
        using IndexIteratorType = typename IndexListType::Iterator;

        // private ctor, can only be called from SparseBinaryDataVectorBase class.
        SparseBinaryDataVectorIterator(const IndexIteratorType& listIterator, size_t size);
        friend SparseBinaryDataVectorBase<IndexListType>;

        // members
        IndexIteratorType _indexIterator;
        size_t _size = 0;
    };

    /// <summary> Implements a sparse binary vector as an increasing list of the coordinates where the
    /// value is 1.0. </summary>
    ///
    /// <typeparam name="tegerListType"> Type of the teger list type. </typeparam>
    template <typename IndexListType>
    class SparseBinaryDataVectorBase : public DataVectorBase<SparseBinaryDataVectorBase<IndexListType>>
    {
    public:
        SparseBinaryDataVectorBase() = default;

        SparseBinaryDataVectorBase(SparseBinaryDataVectorBase<IndexListType>&& other) = default;

        SparseBinaryDataVectorBase(const SparseBinaryDataVectorBase<IndexListType>& other) = delete;

        /// <summary> Constructs a DenseDataVector from an index value iterator. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="indexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept = true>
        SparseBinaryDataVectorBase(IndexValueIteratorType indexValueIterator);

        /// <summary> Constructs a data vector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        SparseBinaryDataVectorBase(std::initializer_list<IndexValue> list);

        /// <summary> Constructs a data vector from an initializer list of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        SparseBinaryDataVectorBase(std::initializer_list<double> list);

        /// <summary> Constructs a data vector from a vector of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        SparseBinaryDataVectorBase(std::vector<IndexValue> vec);

        /// <summary> Constructs a data vector from a vector of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        SparseBinaryDataVectorBase(std::vector<double> vec);

        template <IterationPolicy policy>
        using Iterator = SparseBinaryDataVectorIterator<policy, IndexListType>;

        /// <summary>
        /// Returns an indexValue iterator that points to the beginning of the vector, which iterates
        /// over a prefix of the vector.
        /// </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <param name="size"> The prefix size. </param>
        ///
        /// <returns> The iterator. </returns>
        template<IterationPolicy policy>
        Iterator<policy> GetIterator(size_t size) const;

        /// <summary>
        /// Returns an indexValue iterator that points to the beginning of the vector, which iterates
        /// over a prefix of length PrefixLength().
        /// </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        ///
        /// <returns> The iterator. </returns>
        template<IterationPolicy policy>
        Iterator<policy> GetIterator() const;

        /// <summary> Sets the element at the given index to 1.0. Calls to this function must have a
        /// monotonically increasing argument. The value argument must equal 1.0. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        void AppendElement(size_t index, double value) override;

        /// <summary>
        /// A data vector has infinite dimension and ends with a suffix of zeros. This function returns
        /// the first index in this suffix. Equivalently, the returned value is one plus the index of the
        /// last non-zero element.
        /// </summary>
        ///
        /// <returns> The first index of the suffix of zeros at the end of this vector. </returns>
        size_t PrefixLength() const override;

        /// <summary> Computes the vector squared 2-norm. </summary>
        ///
        /// <returns> The squared 2-norm of the vector. </returns>
        double Norm2Squared() const override { return static_cast<double>(_indexList.Size()); }

        /// <summary> Computes the Dot product. </summary>
        ///
        /// <param name="p_other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        double Dot(math::UnorientedConstVectorBase<double> vector) const override;

        /// <summary> Adds this data vector to a math::RowVector </summary>
        ///
        /// <param name="vector"> [in,out] The vector that this DataVector is added to. </param>
        void AddTo(math::RowVectorReference<double> vector) const override;

    private:
        using DataVectorBase<SparseBinaryDataVectorBase<IndexListType>>::AppendElements;
        IndexListType _indexList;
    };

    /// <summary> A sparse data vector with binary elements. </summary>
    struct SparseBinaryDataVector : public SparseBinaryDataVectorBase<utilities::CompressedIntegerList>
    {
        using SparseBinaryDataVectorBase<utilities::CompressedIntegerList>::SparseBinaryDataVectorBase;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        IDataVector::Type GetType() const override { return IDataVector::Type::SparseBinaryDataVector; }
    };
}
}

#include "../tcc/SparseBinaryDataVector.tcc"

#endif // SPARSEBINARYDATAVECTOR_H
