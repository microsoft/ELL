////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexValue.h"

#include <math/include/Vector.h>

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
} // namespace data
} // namespace ell

#pragma region implementation

#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "SparseDataVector.h"
#include "TransformingIndexValueIterator.h"

namespace ell
{
namespace data
{
    template <typename ReturnType, typename GenericLambdaType>
    ReturnType IDataVector::InvokeWithThis(GenericLambdaType lambda) const
    {
        auto type = GetType();
        switch (type)
        {
        case Type::DoubleDataVector:
            return lambda(static_cast<const DoubleDataVector*>(this));

        case Type::FloatDataVector:
            return lambda(static_cast<const FloatDataVector*>(this));

        case Type::ShortDataVector:
            return lambda(static_cast<const ShortDataVector*>(this));

        case Type::ByteDataVector:
            return lambda(static_cast<const ByteDataVector*>(this));

        case Type::SparseDoubleDataVector:
            return lambda(static_cast<const SparseDoubleDataVector*>(this));

        case Type::SparseFloatDataVector:
            return lambda(static_cast<const SparseFloatDataVector*>(this));

        case Type::SparseShortDataVector:
            return lambda(static_cast<const SparseShortDataVector*>(this));

        case Type::SparseByteDataVector:
            return lambda(static_cast<const SparseByteDataVector*>(this));

        case Type::SparseBinaryDataVector:
            return lambda(static_cast<const SparseBinaryDataVector*>(this));

        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "attempted to cast unsupported data vector type");
        }
    }

    template <IterationPolicy policy, typename TransformationType>
    void IDataVector::AddTransformedTo(math::RowVectorReference<double> vector, TransformationType transformation) const
    {
        InvokeWithThis<void>([vector, transformation](const auto* pThis) {
            pThis->template AddTransformedTo<policy>(vector, transformation);
        });
    }

    template <typename ReturnType>
    ReturnType IDataVector::CopyAs() const
    {
        return InvokeWithThis<ReturnType>([](const auto* pThis) {
            return ReturnType(pThis->template GetIterator<IterationPolicy::skipZeros>());
        });
    }

    template <IterationPolicy policy, typename ReturnType, typename TransformationType>
    ReturnType IDataVector::TransformAs(TransformationType transformation, size_t size) const
    {
        return InvokeWithThis<ReturnType>([transformation, size](const auto* pThis) {
            return ReturnType(MakeTransformingIndexValueIterator(pThis->template GetIterator<policy>(size), transformation));
        });
    }

    template <IterationPolicy policy, typename ReturnType, typename TransformationType>
    ReturnType IDataVector::TransformAs(TransformationType transformation) const
    {
        return InvokeWithThis<ReturnType>([transformation](const auto* pThis) {
            return ReturnType(MakeTransformingIndexValueIterator(pThis->template GetIterator<policy>(), transformation));
        });
    }

    template <class DerivedType>
    template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept>
    void DataVectorBase<DerivedType>::AppendElements(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto current = indexValueIterator.Get();
            static_cast<DerivedType*>(this)->AppendElement(current.index, current.value);
            indexValueIterator.Next();
        }
    }

    template <class DerivedType>
    void DataVectorBase<DerivedType>::AppendElements(std::initializer_list<IndexValue> list)
    {
        for (const auto& current : list)
        {
            static_cast<DerivedType*>(this)->AppendElement(current.index, current.value);
        }
    }

    template <class DerivedType>
    void DataVectorBase<DerivedType>::AppendElements(std::initializer_list<double> list)
    {
        size_t index = 0;
        for (double current : list)
        {
            static_cast<DerivedType*>(this)->AppendElement(index++, current);
        }
    }

    template <class DerivedType>
    void DataVectorBase<DerivedType>::AppendElements(std::vector<IndexValue> vec)
    {
        for (const auto& current : vec)
        {
            static_cast<DerivedType*>(this)->AppendElement(current.index, current.value);
        }
    }

    template <class DerivedType>
    void DataVectorBase<DerivedType>::AppendElements(const std::vector<double>& vec)
    {
        size_t index = 0;
        for (double current : vec)
        {
            static_cast<DerivedType*>(this)->AppendElement(index++, current);
        }
    }

    template <class DerivedType>
    void DataVectorBase<DerivedType>::AppendElements(const std::vector<float>& vec)
    {
        size_t index = 0;
        for (float current : vec)
        {
            static_cast<DerivedType*>(this)->AppendElement(index++, current);
        }
    }

    template <class DerivedType>
    double DataVectorBase<DerivedType>::Norm2Squared() const
    {
        auto iter = GetIterator<DerivedType, IterationPolicy::skipZeros>(*static_cast<const DerivedType*>(this));

        double result = 0.0;
        while (iter.IsValid())
        {
            double value = iter.Get().value;
            result += value * value;
            iter.Next();
        }
        return result;
    }

    template <class DerivedType>
    double DataVectorBase<DerivedType>::Dot(math::UnorientedConstVectorBase<double> vector) const
    {
        auto indexValueIterator = GetIterator<DerivedType, IterationPolicy::skipZeros>(*static_cast<const DerivedType*>(this));

        double result = 0.0;
        auto size = vector.Size();
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            if (indexValue.index >= size)
            {
                break;
            }
            result += indexValue.value * vector[indexValue.index];
            indexValueIterator.Next();
        }
        return result;
    }

    template <class DerivedType>
    float DataVectorBase<DerivedType>::Dot(math::UnorientedConstVectorBase<float> vector) const
    {
        auto indexValueIterator = GetIterator<DerivedType, IterationPolicy::skipZeros>(*static_cast<const DerivedType*>(this));

        float result = 0.0;
        auto size = vector.Size();
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            if (indexValue.index >= size)
            {
                break;
            }
            result += static_cast<float>(indexValue.value) * vector[indexValue.index];
            indexValueIterator.Next();
        }
        return result;
    }

    template <class DerivedType>
    void DataVectorBase<DerivedType>::AddTo(math::RowVectorReference<double> vector) const
    {
        auto indexValueIterator = GetIterator<DerivedType, IterationPolicy::skipZeros>(*static_cast<const DerivedType*>(this));

        auto size = vector.Size();
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            if (indexValue.index >= size)
            {
                return;
            }
            vector[indexValue.index] += indexValue.value;
            indexValueIterator.Next();
        }
    }

    template <class DerivedType>
    std::vector<double> DataVectorBase<DerivedType>::ToArray(size_t size) const
    {
        std::vector<double> result(size);
        auto indexValueIterator = GetIterator<DerivedType, IterationPolicy::skipZeros>(*static_cast<const DerivedType*>(this));

        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            if (indexValue.index >= size)
            {
                break;
            }
            result[indexValue.index] = indexValue.value;
            indexValueIterator.Next();
        }

        return result;
    }

    template <class DerivedType>
    template <IterationPolicy policy, typename TransformationType>
    void DataVectorBase<DerivedType>::AddTransformedTo(math::RowVectorReference<double> vector, TransformationType transformation) const
    {
        auto size = vector.Size();
        auto indexValueIterator = GetIterator<DerivedType, policy>(*static_cast<const DerivedType*>(this), size);

        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            if (indexValue.index >= size)
            {
                return;
            }
            double result = transformation(indexValue);
            vector[indexValue.index] += result;
            indexValueIterator.Next();
        }
    }

    template <class DerivedType>
    template <typename ReturnType>
    ReturnType DataVectorBase<DerivedType>::CopyAs() const
    {
        return ReturnType(GetIterator<DerivedType, IterationPolicy::skipZeros>(*static_cast<const DerivedType*>(this)));
    }

    template <class DerivedType>
    template <IterationPolicy policy, typename ReturnType, typename TransformationType>
    ReturnType DataVectorBase<DerivedType>::TransformAs(TransformationType transformation, size_t size) const
    {
        return ReturnType(MakeTransformingIndexValueIterator(GetIterator<DerivedType, policy>(*static_cast<const DerivedType*>(this), size), std::move(transformation)));
    }

    template <class DerivedType>
    template <IterationPolicy policy, typename ReturnType, typename TransformationType>
    ReturnType DataVectorBase<DerivedType>::TransformAs(TransformationType transformation) const
    {
        return ReturnType(MakeTransformingIndexValueIterator(GetIterator<DerivedType, policy>(*static_cast<const DerivedType*>(this)), std::move(transformation)));
    }

    template <class DerivedType>
    void DataVectorBase<DerivedType>::Print(std::ostream& os) const
    {
        auto indexValueIterator = GetIterator<DerivedType, IterationPolicy::skipZeros>(*static_cast<const DerivedType*>(this));
        if (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            os << indexValue.index << ":" << indexValue.value;
            indexValueIterator.Next();
        }

        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            os << '\t' << indexValue.index << ":" << indexValue.value;
            indexValueIterator.Next();
        }
    }

    template <typename DataVectorType, IterationPolicy policy, typename TransformationType>
    static void AddTransformedTo(const DataVectorType& dataVector, math::RowVectorReference<double> vector, TransformationType transformation)
    {
        return dataVector.template AddTransformedTo<policy, TransformationType>(vector, transformation);
    }

    template <typename DataVectorType, IterationPolicy policy>
    static auto GetIterator(DataVectorType& vector)
    {
        return vector.template GetIterator<policy>();
    }

    template <typename DataVectorType, IterationPolicy policy>
    static auto GetIterator(const DataVectorType& vector)
    {
        return vector.template GetIterator<policy>();
    }

    template <typename DataVectorType, IterationPolicy policy>
    static auto GetIterator(DataVectorType& vector, size_t size)
    {
        return vector.template GetIterator<policy>(size);
    }

    template <typename DataVectorType, IterationPolicy policy>
    static auto GetIterator(const DataVectorType& vector, size_t size)
    {
        return vector.template GetIterator<policy>(size);
    }

    template <typename DataVectorType, typename ReturnType>
    static ReturnType CopyAs(DataVectorType& vector)
    {
        return vector.template CopyAs<ReturnType>();
    }

    template <typename DataVectorType, typename ReturnType>
    static ReturnType CopyAs(const DataVectorType& vector)
    {
        return vector.template CopyAs<ReturnType>();
    }

    template <typename DataVectorType, IterationPolicy policy, typename ReturnType, typename TransformationType>
    static ReturnType TransformAs(DataVectorType& vector, TransformationType transformation, size_t size)
    {
        return vector.template TransformAs<policy, ReturnType, TransformationType>(transformation, size);
    }

    template <typename DataVectorType, IterationPolicy policy, typename ReturnType, typename TransformationType>
    static ReturnType TransformAs(const DataVectorType& vector, TransformationType transformation, size_t size)
    {
        return vector.template TransformAs<policy, ReturnType, TransformationType>(transformation, size);
    }

    template <typename DataVectorType, IterationPolicy policy, typename ReturnType, typename TransformationType>
    static ReturnType TransformAs(DataVectorType& vector, TransformationType transformation)
    {
        return vector.template TransformAs<policy, ReturnType, TransformationType>(transformation);
    }

    template <typename DataVectorType, IterationPolicy policy, typename ReturnType, typename TransformationType>
    static ReturnType TransformAs(const DataVectorType& vector, TransformationType transformation)
    {
        return vector.template TransformAs<policy, ReturnType, TransformationType>(transformation);
    }
} // namespace data
} // namespace ell

#pragma endregion implementation
