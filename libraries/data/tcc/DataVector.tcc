////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
        InvokeWithThis<void>([vector, transformation](const auto* pThis)
        {
            pThis->template AddTransformedTo<policy>(vector, transformation);
        });
    }

    template <typename ReturnType>
    ReturnType IDataVector::CopyAs() const
    {
        return InvokeWithThis<ReturnType>([](const auto* pThis)
        {
            return ReturnType(pThis->template GetIterator<IterationPolicy::skipZeros>());
        });
    }

    template <IterationPolicy policy, typename ReturnType, typename TransformationType>
    ReturnType IDataVector::TransformAs(TransformationType transformation, size_t size) const
    {
        return InvokeWithThis<ReturnType>([transformation, size](const auto* pThis)
        {
            return ReturnType(MakeTransformingIndexValueIterator(pThis->template GetIterator<policy>(size), transformation));
        });
    }

    template <IterationPolicy policy, typename ReturnType, typename TransformationType>
    ReturnType IDataVector::TransformAs(TransformationType transformation) const
    {
        return InvokeWithThis<ReturnType>([transformation](const auto* pThis)
        {
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
}
}
