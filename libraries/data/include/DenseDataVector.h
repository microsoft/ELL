////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DenseDataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVector.h"
#include "StlIndexValueIterator.h"

#ifndef DENSEDATAVECTOR_H
#define DENSEDATAVECTOR_H

#include <utilities/include/StlContainerIterator.h>

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <vector>

#define DEFAULT_DENSE_VECTOR_CAPACITY 256

namespace ell
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
        /// <summary> Constructor. </summary>
        DenseDataVector();

        DenseDataVector(DenseDataVector&& other) = default;

        DenseDataVector(const DenseDataVector&) = delete;

        /// <summary> Constructs a DenseDataVector from an index value iterator. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept = true>
        DenseDataVector(IndexValueIteratorType indexValueIterator);

        /// <summary> Constructs a data vector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        DenseDataVector(std::initializer_list<IndexValue> list);

        /// <summary> Constructs a data vector from an initializer list of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        DenseDataVector(std::initializer_list<double> list);

        /// <summary> Constructs a data vector from a `std::vector` of values. </summary>
        ///
        /// <param name="list"> The vector of values. </param>
        DenseDataVector(std::vector<IndexValue> vec);

        /// <summary> Constructs a data vector from a `std::vector` of values. </summary>
        ///
        /// <param name="list"> The vector of values. </param>
        DenseDataVector(std::vector<double> vec);

        /// <summary> Constructs a data vector from a `std::vector` of values. </summary>
        ///
        /// <param name="list"> The vector of values. </param>
        DenseDataVector(std::vector<float> vec);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the desired element. </param>
        ///
        /// <returns> Value of the desired element. </returns>
        double operator[](size_t index) const;

        /// <summary>
        /// Returns an indexValue iterator that points to the beginning of the vector, which iterates
        /// over a prefix of the vector.
        /// </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <param name="size"> The prefix size. </param>
        ///
        /// <returns> The iterator. </returns>
        template <IterationPolicy policy>
        VectorIndexValueIterator<policy, ElementType> GetIterator(size_t size) const;

        /// <summary>
        /// Returns an indexValue iterator that points to the beginning of the vector, which iterates
        /// over a prefix of length PrefixLength().
        /// </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        ///
        /// <returns> The iterator. </returns>
        template <IterationPolicy policy>
        VectorIndexValueIterator<policy, ElementType> GetIterator() const;

        /// <summary> Appends an element to the end of the data vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the element, must be bigger than the biggest current index. </param>
        /// <param name="value"> The element value. </param>
        void AppendElement(size_t index, double value) override;

        /// <summary>
        /// A data vector has infinite dimension and ends with a suffix of zeros. This function returns
        /// the first index in this suffix. Equivalently, the returned value is one plus the index of the
        /// last non-zero element.
        /// </summary>
        ///
        /// <returns> The first index of the suffix of zeros at the end of this vector. </returns>
        size_t PrefixLength() const override { return _data.size(); }

        /// <summary> Gets the data vector type (implemented by template specialization). </summary>
        ///
        /// <returns> The data vector type. </returns>
        IDataVector::Type GetType() const override
        {
            return GetStaticType();
        }

        static IDataVector::Type GetStaticType();

    private:
        using DataVectorBase<DenseDataVector<ElementType>>::AppendElements;
        size_t _numNonzeros = 0;
        std::vector<ElementType> _data;
    };

    /// <summary> A dense data vector with double elements. </summary>
    using DoubleDataVector = DenseDataVector<double>;

    /// <summary> A dense data vector with float elements. </summary>
    using FloatDataVector = DenseDataVector<float>;

    /// <summary> A dense data vector with short elements. </summary>
    using ShortDataVector = DenseDataVector<short>;

    /// <summary> A dense data vector with byte elements. </summary>
    using ByteDataVector = DenseDataVector<char>;
} // namespace data
} // namespace ell

#pragma region implementation

#include <utilities/include/Exception.h>
#include <utilities/include/StringUtil.h>
#include <utilities/include/TypeName.h>

#include <cassert>

namespace ell
{
namespace data
{
    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector() :
        _numNonzeros(0)
    {
        _data.reserve(DEFAULT_DENSE_VECTOR_CAPACITY);
    }

    template <typename ElementType>
    template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept>
    DenseDataVector<ElementType>::DenseDataVector(IndexValueIteratorType indexValueIterator)
    {
        AppendElements(std::move(indexValueIterator));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::initializer_list<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::initializer_list<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::vector<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::vector<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::vector<float> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    double DenseDataVector<ElementType>::operator[](size_t index) const
    {
        if (index >= _data.size())
        {
            return 0.0;
        }
        return static_cast<double>(_data[index]);
    }

    template <typename ElementType>
    template <IterationPolicy policy>
    VectorIndexValueIterator<policy, ElementType> DenseDataVector<ElementType>::GetIterator(size_t size) const
    {
        return MakeVectorIndexValueIterator<policy>(_data, size);
    }

    template <typename ElementType> // move this to datavectorbase?
    template <IterationPolicy policy>
    VectorIndexValueIterator<policy, ElementType> DenseDataVector<ElementType>::GetIterator() const
    {
        return GetIterator<policy>(PrefixLength());
    }

    template <typename ElementType>
    void DenseDataVector<ElementType>::AppendElement(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        ElementType storedValue = static_cast<ElementType>(value);

        if (storedValue - value > 1.0e-5 || value - storedValue > 1.0e-5)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            utilities::FormatString("Data loss detected when storing value %f as type %s", value, utilities::GetTypeName<ElementType>().c_str()));
        }

        if (index < _data.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
        }

        _data.resize(index + 1);
        _data[index] = storedValue;
        ++_numNonzeros;
    }
} // namespace data
} // namespace ell

#pragma endregion implementation

#endif // DENSEDATAVECTOR_H
