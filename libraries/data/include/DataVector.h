////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexValue.h"

// math
#include "Vector.h"

// stl
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <vector>

namespace ell
{
namespace data
{
    /// <summary> Interface for infinite-dimensional vectors of double numbers. Each implementation of
    /// this interface has a mathematical dimension of infinity and is made up of an explicitly specified
    /// prefix followed by an implicit suffix of zeros. </summary>
    class IDataVector
    {
    public:
        /// <summary> Values that represent data vector types. </summary>
        enum class Type
        {
            DoubleDataVector,
            FloatDataVector,
            ShortDataVector,
            ByteDataVector,
            SparseDoubleDataVector,
            SparseFloatDataVector,
            SparseShortDataVector,
            SparseByteDataVector,
            SparseBinaryDataVector,
            AutoDataVector
        };

        virtual ~IDataVector() = default;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual Type GetType() const = 0;

        /// <summary> Adds a value at the end of the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendElement(size_t index, double value) = 0;

        /// <summary>
        /// A data vector has infinite dimension and ends with a suffix of zeros. This function returns
        /// the first index in this suffix. Equivalently, the returned value is one plus the index of the
        /// last non-zero element.
        /// </summary>
        ///
        /// <returns> The first index of the suffix of zeros at the end of this vector. </returns>
        virtual size_t PrefixLength() const = 0;

        /// <summary> Computes the squared 2-norm of the vector. </summary>
        ///
        /// <returns> The squared 2-norm of the vector. </returns>
        virtual double Norm2Squared() const = 0;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual double Dot(math::UnorientedConstVectorBase<double> vector) const = 0;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual float Dot(math::UnorientedConstVectorBase<float> vector) const = 0;

        /// <summary> Adds this data vector to a math::RowVector </summary>
        ///
        /// <param name="vector"> [in,out] The vector to which this data vector is added. </param>
        virtual void AddTo(math::RowVectorReference<double> vector) const = 0;

        /// <summary> Adds a transformed version of this data vector to a math::RowVector. </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <typeparam name="TransformationType"> Non zero transformation type, which is a functor that
        /// takes an IndexValue and returns a double, and is applied to each element of the vector. </typeparam>
        /// <param name="vector"> The vector. </param>
        /// <param name="transformation"> The transformation.. </param>
        template <IterationPolicy policy, typename TransformationType>
        void AddTransformedTo(math::RowVectorReference<double> vector, TransformationType transformation) const;

        /// <summary> Copies the contents of this DataVector into a double array of size PrefixLength(). </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const = 0;

        /// <summary> Copies the contents of this DataVector into a double array of a given size. </summary>
        ///
        /// <param name="size"> The desired array size. </param>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray(size_t size) const = 0;

        /// <summary> Copies this data vector into another type of data vector. </summary>
        ///
        /// <typeparam name="ReturnType"> The return type. </typeparam>
        ///
        /// <returns> The new data vector. </returns>
        template <typename ReturnType>
        ReturnType CopyAs() const;

        /// <summary> Copies a transformed version of a prefix of this data vector. </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
        /// <typeparam name="TransformationType"> The transformation type. </typeparam>
        /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
        /// applied to the elements of the data vector. </param>
        /// <param name="size"> The prefix size. </param>
        ///
        /// <returns> The transformed data vector. </returns>
        template <IterationPolicy policy, typename ReturnType, typename TransformationType>
        ReturnType TransformAs(TransformationType transformation, size_t size) const;

        /// <summary> Copies a transformed version of this data vector. </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
        /// <typeparam name="TransformationType"> The transformation type. </typeparam>
        /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
        /// applied to the elements of the data vector. </param>
        ///
        /// <returns> The transformed data vector. </returns>
        template <IterationPolicy policy, typename ReturnType, typename TransformationType>
        ReturnType TransformAs(TransformationType transformation) const;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        virtual void Print(std::ostream& os) const = 0;

    private:
        // calls a generic (polymorphic) lambda of the form `[](const auto* pThis){ return ReturnType();}`, where pThis is a pointer to the concrete
        // DataVector implementation (e.g., DenseDataVector, SparseDataVector,...)
        template <typename ReturnType, typename GenericLambdaType>
        ReturnType InvokeWithThis(GenericLambdaType lambda) const;
    };

    /// <summary> A helper definition used to define the IsDataVector SFINAE concept. </summary>
    template <typename T>
    using IsDataVector = typename std::enable_if_t<std::is_base_of<IDataVector, T>::value, bool>;

    /// <summary> Adds a DataVector to a math::RowVector. </summary>
    ///
    /// <param name="vector"> The math::RowVector being modified. </param>
    /// <param name="scaledDataVector"> The DataVector being added to the vector. </param>
    void operator+=(math::RowVectorReference<double> vector, const IDataVector& dataVector);

    /// <summary>
    /// Base class for some of the data vector classes. This class uses a curiously recurring
    /// template pattern to significantly reduce code duplication in the derived classes.
    /// </summary>
    ///
    /// <typeparam name="DerivedType"> The derived type, in the curiously recurring template design pattern. </typeparam>
    template <class DerivedType>
    class DataVectorBase : public IDataVector
    {
    public:
        /// <summary> Takes an iterator and appends its entries to the data vector. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept = true>
        void AppendElements(IndexValueIteratorType indexValueIterator);

        /// <summary> Takes an initializer list of index value pairs and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The initializer list of index value pairs. </param>
        void AppendElements(std::initializer_list<IndexValue> list);

        /// <summary> Takes an initializer list of values and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        void AppendElements(std::initializer_list<double> list);

        /// <summary> Takes a vector of index value pairs and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The vector of index value pairs. </param>
        void AppendElements(std::vector<IndexValue> vec);

        /// <summary> Takes a vector of values and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The vector. </param>
        void AppendElements(const std::vector<double>& vec);

        /// <summary> Takes a vector of values and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The vector. </param>
        void AppendElements(const std::vector<float>& vec);

        /// <summary> Computes the 2-norm of the vector. </summary>
        ///
        /// <returns> The squared 2-norm of the vector. </returns>
        double Norm2Squared() const override;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        double Dot(math::UnorientedConstVectorBase<double> vector) const override;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        float Dot(math::UnorientedConstVectorBase<float> vector) const override;

        /// <summary> Adds this data vector to a math::RowVector </summary>
        ///
        /// <param name="vector"> [in,out] The vector to which this data vector is added. </param>
        void AddTo(math::RowVectorReference<double> vector) const override;

        /// <summary> Adds a transformed version of this data vector to a math::RowVector. </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <typeparam name="TransformationType"> Non zero transformation type, which is a functor that
        /// takes an IndexValue and returns a double, and is applied to each element of the vector. </typeparam>
        /// <param name="vector"> The vector. </param>
        /// <param name="transformation"> The transformation.. </param>
        template <IterationPolicy policy, typename TransformationType>
        void AddTransformedTo(math::RowVectorReference<double> vector, TransformationType transformation) const;

        /// <summary> Returns a (dense) iterator of the vector elements, excluding the final suffix of zeros. </summary>
        ///
        /// <returns> A value iterator. </returns>
        auto GetValueIterator() { return static_cast<DerivedType*>(this)->GetValueIterator(PrefixLength()); }

        /// <summary> Copies the contents of this DataVector into a double array of size PrefixLength(). </summary>
        ///
        /// <returns> The array. </returns>
        std::vector<double> ToArray() const override { return ToArray(PrefixLength()); }

        /// <summary> Copies the contents of this DataVector into a double array of a given size. </summary>
        ///
        /// <param name="size"> The desired array size. </param>
        ///
        /// <returns> The array. </returns>
        std::vector<double> ToArray(size_t size) const override;

        /// <summary> Copies the contents of a data vector to another data vector. </summary>
        ///
        /// <typeparam name="ReturnType"> Type of the data vector to construct. </typeparam>
        ///
        /// <returns> A data vector of a specified type. </returns>
        template <typename ReturnType>
        ReturnType CopyAs() const;

        /// <summary> Copies a transformed version of a prefix of this data vector. </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
        /// <typeparam name="TransformationType"> The transformation type. </typeparam>
        /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
        /// applied to the elements of the data vector. </param>
        /// <param name="size"> The prefix size. </param>
        ///
        /// <returns> The transformed data vector. </returns>
        template <IterationPolicy policy, typename ReturnType, typename TransformationType>
        ReturnType TransformAs(TransformationType transformation, size_t size) const;

        /// <summary> Copies a transformed version of this data vector. </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
        /// <typeparam name="TransformationType"> The transformation type. </typeparam>
        /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
        /// applied to the elements of the data vector. </param>
        ///
        /// <returns> The transformed data vector. </returns>
        template <IterationPolicy policy, typename ReturnType, typename TransformationType>
        ReturnType TransformAs(TransformationType transformation) const;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        void Print(std::ostream& os) const override;
    };

    /// <summary> Wrapper for AddTransformedTo that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call AddTransformedTo on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <typeparam name="TransformationType"> Non zero transformation type, which is a functor that
    /// takes an IndexValue and returns a double, and is applied to each element of the vector. </typeparam>
    /// <param name="vector"> The data vector that we're calling AddTransformedTo. </param>
    /// <param name="vector"> The vector. </param>
    /// <param name="transformation"> The transformation.. </param>
    template <typename DataVectorType, IterationPolicy policy, typename TransformationType>
    static void AddTransformedTo(const DataVectorType& dataVector, math::RowVectorReference<double> vector, TransformationType transformation);

    /// <summary> Wrapper for GetIterator that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call GetIterator on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <param name="vector"> The data vector that we're getting the iterator from. </param>
    ///
    /// <returns> The iterator. </returns>
    template <typename DataVectorType, IterationPolicy policy>
    static auto GetIterator(DataVectorType& vector);

    /// <summary> Wrapper for GetIterator that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call GetIterator on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <param name="vector"> The const data vector that we're getting the iterator from. </param>
    ///
    /// <returns> The iterator. </returns>
    template <typename DataVectorType, IterationPolicy policy>
    static auto GetIterator(const DataVectorType& vector);

    /// <summary> Wrapper for GetIterator that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call GetIterator on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <param name="vector"> The data vector that we're getting the iterator from. </param>
    /// <param name="size"> The size of the vector. </param>
    ///
    /// <returns> The iterator. </returns>
    template <typename DataVectorType, IterationPolicy policy>
    static auto GetIterator(DataVectorType& vector, size_t size);

    /// <summary> Wrapper for GetIterator that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call GetIterator on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <param name="vector"> The const data vector that we're getting the iterator from. </param>
    /// <param name="size"> The size of the vector. </param>
    ///
    /// <returns> The iterator. </returns>
    template <typename DataVectorType, IterationPolicy policy>
    static auto GetIterator(const DataVectorType& vector, size_t size);

    /// <summary> Wrapper for CopyAs that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call CopyAs on </typeparam>
    /// <typeparam name="ReturnType"> Type of the data vector to construct. </typeparam>
    /// <param name="vector"> The data vector that we're copying. </param>
    ///
    /// <returns> A data vector of a specified type. </returns>
    template <typename DataVectorType, typename ReturnType>
    static ReturnType CopyAs(DataVectorType& vector);

    /// <summary> Wrapper for CopyAs that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call CopyAs on </typeparam>
    /// <typeparam name="ReturnType"> Type of the data vector to construct. </typeparam>
    /// <param name="vector"> The const data vector that we're copying. </param>
    ///
    /// <returns> A data vector of a specified type. </returns>
    template <typename DataVectorType, typename ReturnType>
    static ReturnType CopyAs(const DataVectorType& vector);

    /// <summary> Wrapper for TransformAs that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call TransformAs on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="vector"> The data vector that we're transforming. </param>
    /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
    /// applied to the elements of the data vector. </param>
    /// <param name="size"> The prefix size. </param>
    ///
    /// <returns> The transformed data vector. </returns>
    template <typename DataVectorType, IterationPolicy policy, typename ReturnType, typename TransformationType>
    static ReturnType TransformAs(DataVectorType& vector, TransformationType transformation, size_t size);

    /// <summary> Wrapper for TransformAs that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call TransformAs on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="vector"> The const data vector that we're transforming. </param>
    /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
    /// applied to the elements of the data vector. </param>
    /// <param name="size"> The prefix size. </param>
    ///
    /// <returns> The transformed data vector. </returns>
    template <typename DataVectorType, IterationPolicy policy, typename ReturnType, typename TransformationType>
    static ReturnType TransformAs(const DataVectorType& vector, TransformationType transformation, size_t size);

    /// <summary> Wrapper for TransformAs that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call TransformAs on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="vector"> The data vector that we're transforming. </param>
    /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
    /// applied to the elements of the data vector. </param>
    ///
    /// <returns> The transformed data vector. </returns>
    template <typename DataVectorType, IterationPolicy policy, typename ReturnType, typename TransformationType>
    static ReturnType TransformAs(DataVectorType& vector, TransformationType transformation);

    /// <summary> Wrapper for TransformAs that hides the template specifier. </summary>
    ///
    /// <typeparam name="DataVectorType"> The data vector type to call TransformAs on </typeparam>
    /// <typeparam name="policy"> The iteration policy. </typeparam>
    /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="vector"> The const data vector that we're transforming. </param>
    /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
    /// applied to the elements of the data vector. </param>
    ///
    /// <returns> The transformed data vector. </returns>
    template <typename DataVectorType, IterationPolicy policy, typename ReturnType, typename TransformationType>
    static ReturnType TransformAs(const DataVectorType& vector, TransformationType transformation);
}
}

#include "../tcc/DataVector.tcc"