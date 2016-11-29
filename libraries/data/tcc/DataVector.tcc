////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "SparseDataVector.h"
#include "TransformingIndexValueIterator.h"

//stl
#include <cmath>

namespace emll
{
namespace data
{
    template <typename ReturnType>
    ReturnType IDataVector::DeepCopyAs() const
    {
        switch (GetType())
        {
            case Type::DoubleDataVector:
                return ReturnType(static_cast<const DoubleDataVector*>(this)->GetIterator());

            case Type::FloatDataVector:
                return ReturnType(static_cast<const FloatDataVector*>(this)->GetIterator());

            case Type::ShortDataVector:
                return ReturnType(static_cast<const ShortDataVector*>(this)->GetIterator());

            case Type::ByteDataVector:
                return ReturnType(static_cast<const ByteDataVector*>(this)->GetIterator());

            case Type::SparseDoubleDataVector:
                return ReturnType(static_cast<const SparseDoubleDataVector*>(this)->GetIterator());

            case Type::SparseFloatDataVector:
                return ReturnType(static_cast<const SparseFloatDataVector*>(this)->GetIterator());

            case Type::SparseShortDataVector:
                return ReturnType(static_cast<const SparseShortDataVector*>(this)->GetIterator());

            case Type::SparseByteDataVector:
                return ReturnType(static_cast<const SparseByteDataVector*>(this)->GetIterator());

            case Type::SparseBinaryDataVector:
                return ReturnType(static_cast<const SparseBinaryDataVector*>(this)->GetIterator());

            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "attempted to cast unsupported data vector type");
        }
    }

    template <typename ReturnType, typename TransformType>
    ReturnType IDataVector::DeepCopyAs(TransformType nonZeroTransform) const
    {
        switch (GetType())
        {
            case Type::DoubleDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const DoubleDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            case Type::FloatDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const FloatDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            case Type::ShortDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const ShortDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            case Type::ByteDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const ByteDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            case Type::SparseDoubleDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const SparseDoubleDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            case Type::SparseFloatDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const SparseFloatDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            case Type::SparseShortDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const SparseShortDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            case Type::SparseByteDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const SparseByteDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            case Type::SparseBinaryDataVector:
                return ReturnType(MakeTransformingIndexValueIterator(static_cast<const SparseBinaryDataVector*>(this)->GetIterator(), std::move(nonZeroTransform)));

            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "attempted to cast unsupported data vector type");
        }
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
    void DataVectorBase<DerivedType>::AppendElements(std::vector<double> vec)
    {
        size_t index = 0;
        for (double current : vec)
        {
            static_cast<DerivedType*>(this)->AppendElement(index++, current);
        }
    }

    template <class DerivedType>
    double DataVectorBase<DerivedType>::Norm2() const
    {
        auto iter = static_cast<const DerivedType*>(this)->GetIterator();

        double result = 0.0;
        while (iter.IsValid())
        {
            double value = iter.Get().value;
            result += value * value;
            iter.Next();
        }
        return std::sqrt(result);
    }

    template <class DerivedType>
    double DataVectorBase<DerivedType>::Dot(const math::UnorientedConstVectorReference<double>& vector) const
    {
        auto indexValueIterator = static_cast<const DerivedType*>(this)->GetIterator();

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
    void DataVectorBase<DerivedType>::AddTo(math::RowVectorReference<double>& vector, double scalar) const
    {
        auto indexValueIterator = static_cast<const DerivedType*>(this)->GetIterator();

        auto originalSize = vector.Size();
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            if (indexValue.index >= originalSize)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "vector size is smaller than data vector prefix length");
            }
            vector[indexValue.index] += scalar * indexValue.value;
            indexValueIterator.Next();
        }
    }

    template <class DerivedType>
    std::vector<double> DataVectorBase<DerivedType>::ToArray() const
    {
        auto size = static_cast<const DerivedType*>(this)->PrefixLength();
 
        std::vector<double> result(size);
        auto indexValueIterator = static_cast<const DerivedType*>(this)->GetIterator();

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
    std::vector<double> DataVectorBase<DerivedType>::ToArray(size_t size) const
    {
        std::vector<double> result(size);
        auto indexValueIterator = static_cast<const DerivedType*>(this)->GetIterator();

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
    template <typename ReturnType>
    ReturnType DataVectorBase<DerivedType>::DeepCopyAs() const
    {
        return ReturnType(static_cast<const DerivedType*>(this)->GetIterator());
    }

    template <class DerivedType>
    template <typename ReturnType, typename TransformType>
    ReturnType DataVectorBase<DerivedType>::DeepCopyAs(TransformType nonZeroTransform) const
    {
        return ReturnType(MakeTransformingIndexValueIterator(static_cast<const DerivedType*>(this)->GetIterator(), std::move(nonZeroTransform)));
    }

    template <class DerivedType>
    void DataVectorBase<DerivedType>::Print(std::ostream& os) const
    {
        auto indexValueIterator = static_cast<const DerivedType*>(this)->GetIterator();
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
}
}