////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//stl 
#include <cmath>

namespace emll
{
namespace dataset
{
    template<typename ReturnType>
    ReturnType IDataVector::ToDataVector() const
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
    double DataVectorBase<DerivedType>::Dot(const double * p_other) const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        double result = 0.0;
        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            result += indexValue.value * p_other[indexValue.index];
            constIter.Next();
        }
        return result;
    }

    template<class DerivedType>
    void DataVectorBase<DerivedType>::AddTo(double * p_other, double scalar) const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            p_other[indexValue.index] += scalar * indexValue.value;
            constIter.Next();
        }
    }

    template<class DerivedType>
    std::vector<double> DataVectorBase<DerivedType>::ToArray() const
    {
        std::vector<double> result(static_cast<const DerivedType*>(this)->Size());
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
    ReturnType DataVectorBase<DerivedType>::ToDataVector() const
    {
        return ReturnType(static_cast<const DerivedType*>(this)->GetIterator());
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