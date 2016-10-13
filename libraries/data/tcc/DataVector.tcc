////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//stl 
#include <cmath>
#include "..\include\DataVector.h"

namespace emll
{
namespace data
{
    template<typename ReturnType>
    ReturnType IDataVector::Duplicate(std::function<double(IndexValue)> nonZeroMapper) const
    {
        switch (GetType())
        {
        case Type::DoubleDataVector:
            return ReturnType(static_cast<const DoubleDataVector*>(this)->GetIterator(), nonZeroMapper);

        case Type::FloatDataVector:
            return ReturnType(static_cast<const FloatDataVector*>(this)->GetIterator(), nonZeroMapper);

        case Type::ShortDataVector:
            return ReturnType(static_cast<const ShortDataVector*>(this)->GetIterator(), nonZeroMapper);

        case Type::ByteDataVector:
            return ReturnType(static_cast<const ByteDataVector*>(this)->GetIterator(), nonZeroMapper);

        case Type::SparseDoubleDataVector:
            return ReturnType(static_cast<const SparseDoubleDataVector*>(this)->GetIterator(), nonZeroMapper);

        case Type::SparseFloatDataVector:
            return ReturnType(static_cast<const SparseFloatDataVector*>(this)->GetIterator(), nonZeroMapper);

        case Type::SparseShortDataVector:
            return ReturnType(static_cast<const SparseShortDataVector*>(this)->GetIterator(), nonZeroMapper);

        case Type::SparseByteDataVector:
            return ReturnType(static_cast<const SparseByteDataVector*>(this)->GetIterator(), nonZeroMapper);

        case Type::SparseBinaryDataVector:
            return ReturnType(static_cast<const SparseBinaryDataVector*>(this)->GetIterator(), nonZeroMapper);

        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "attempted to cast unsupported data vector type");
        }
    }

    template<class DerivedType>
    template<typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept>
    void DataVectorBase<DerivedType>::AppendElements(IndexValueIteratorType indexValueIterator, std::function<double(IndexValue)> nonZeroMapper)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            double value = nonZeroMapper ? nonZeroMapper(indexValue) : indexValue.value;
            static_cast<DerivedType*>(this)->AppendElement(indexValue.index, value);
            indexValueIterator.Next();
        }
    }

    template<class DerivedType>
    void DataVectorBase<DerivedType>::AppendElements(std::initializer_list<IndexValue> list)
    {
        auto current = list.begin();
        auto end = list.end();
        while (current < end)
        {
            static_cast<DerivedType*>(this)->AppendElement(current->index, current->value);
            ++current;
        }
    }

    template<class DerivedType>
    void DataVectorBase<DerivedType>::AppendElements(std::initializer_list<double> list)
    {
        auto current = list.begin();
        auto end = list.end();
        size_t index = 0;
        while (current < end)
        {
            static_cast<DerivedType*>(this)->AppendElement(index, *current);
            ++current;
            ++index;
        }
    }

    template<class DerivedType>
    double DataVectorBase<DerivedType>::Norm2() const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        double result = 0.0;
        while(constIter.IsValid())
        {
            double value = constIter.Get().value;
            result += value * value;
            constIter.Next();
        }
        return std::sqrt(result);
    }

    template<class DerivedType>
    double DataVectorBase<DerivedType>::Dot(const math::UnorientedConstVectorReference<double>& vector) const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        double result = 0.0;
        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            result += indexValue.value * vector[indexValue.index];
            constIter.Next();
        }
        return result;
    }

    template<class DerivedType>
    void DataVectorBase<DerivedType>::AddTo(math::RowVectorReference<double>& vector, double scalar) const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            vector[indexValue.index] += scalar * indexValue.value;
            constIter.Next();
        }
    }

    template<class DerivedType>
    std::vector<double> DataVectorBase<DerivedType>::ToArray() const
    {
        std::vector<double> result(static_cast<const DerivedType*>(this)->ZeroSuffixFirstIndex());
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            result[indexValue.index] = indexValue.value;
            constIter.Next();
        }

        return result;
    }

    template<class DerivedType>
    template<typename ReturnType>
    ReturnType DataVectorBase<DerivedType>::Duplicate(std::function<double(IndexValue)> nonZeroMapper) const
    {
        return ReturnType(static_cast<const DerivedType*>(this)->GetIterator(), std::move(nonZeroMapper));
    }

    template<class DerivedType>
    void DataVectorBase<DerivedType>::Print(std::ostream & os) const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();
        if(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            os << indexValue.index << ":" << indexValue.value;
        }
        constIter.Next();
        
        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            os << '\t' << indexValue.index << ":" << indexValue.value;
            constIter.Next();
        }
    }
}
}