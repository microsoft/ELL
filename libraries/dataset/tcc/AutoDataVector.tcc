////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AutoDataVectorBase.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#define APPROXIMATION_TOLERANCE 1.0e-9
#define SPARSE_THRESHOLD 0.2

#include "DenseDataVector.h"
#include "SparseDataVector.h"
#include "SparseBinaryDataVector.h"

namespace emll
{
namespace dataset
{
    template<typename DefaultDataVectorType>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(DefaultDataVectorType&& vector)
    {
        FindBestRepresentation(std::move(vector));
    }

    template<typename DefaultDataVectorType>
    template<typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> Concept>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(IndexValueIteratorType indexValueIterator)
    {
        DefaultDataVectorType defaultDataVector(indexValueIterator);
        FindBestRepresentation(std::move(defaultDataVector));
    }

    template<typename DefaultDataVectorType>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(std::initializer_list<linear::IndexValue> list)
    {
        DefaultDataVectorType defaultDataVector(list);
        FindBestRepresentation(std::move(defaultDataVector));
    }

    template<typename DefaultDataVectorType>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(std::initializer_list<double> list)
    {
        DefaultDataVectorType defaultDataVector(list);
        FindBestRepresentation(std::move(defaultDataVector));
    }

    template<typename DefaultDataVectorType>
    void AutoDataVectorBase<DefaultDataVectorType>::AppendElement(size_t index, double value)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Append element not supported for AutoDataVector");
    }

    template<typename DefaultDataVectorType>
    double AutoDataVectorBase<DefaultDataVectorType>::Dot(const double * p_other) const
    {
        return _pInternal->Dot(p_other);
    }

    template<typename DefaultDataVectorType>
    void AutoDataVectorBase<DefaultDataVectorType>::AddTo(double * p_other, double scalar) const
    {
        _pInternal->AddTo(p_other, scalar);
    }

    template<typename DefaultDataVectorType>
    void AutoDataVectorBase<DefaultDataVectorType>::Print(std::ostream & os) const
    {
        _pInternal->Print(os);
    }

    template<typename DefaultDataVectorType>
    template<typename ReturnType>
    ReturnType AutoDataVectorBase<DefaultDataVectorType>::ToDataVector() const
    {
        switch (_pInternal->GetType())
        {
        case IDataVector::Type::DoubleDataVector:
            return ReturnType(static_cast<DoubleDataVector*>(_pInternal.get())->GetIterator());

        case IDataVector::Type::FloatDataVector:
            return ReturnType(static_cast<FloatDataVector*>(_pInternal.get())->GetIterator());

        case IDataVector::Type::ShortDataVector:
            return ReturnType(static_cast<ShortDataVector*>(_pInternal.get())->GetIterator());

        case IDataVector::Type::ByteDataVector:
            return ReturnType(static_cast<ByteDataVector*>(_pInternal.get())->GetIterator());

        case IDataVector::Type::SparseDoubleDataVector:
            return ReturnType(static_cast<SparseDoubleDataVector*>(_pInternal.get())->GetIterator());

        case IDataVector::Type::SparseFloatDataVector:
            return ReturnType(static_cast<SparseFloatDataVector*>(_pInternal.get())->GetIterator());

        case IDataVector::Type::SparseShortDataVector:
            return ReturnType(static_cast<SparseShortDataVector*>(_pInternal.get())->GetIterator());

        case IDataVector::Type::SparseByteDataVector:
            return ReturnType(static_cast<SparseByteDataVector*>(_pInternal.get())->GetIterator());

        case IDataVector::Type::SparseBinaryDataVector:
            return ReturnType(static_cast<SparseBinaryDataVector*>(_pInternal.get())->GetIterator());

        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "attempted to cast unsupported data vector type");
        }

        return ReturnType();
    }

    template<typename TargetType>
    bool DoesCastModifyValue(double value)
    {
        double target = static_cast<double>(static_cast<TargetType>(value));
        return (target - value > APPROXIMATION_TOLERANCE) || (value - target > APPROXIMATION_TOLERANCE);
    }

    template<typename DefaultDataVectorType>
    void AutoDataVectorBase<DefaultDataVectorType>::FindBestRepresentation(DefaultDataVectorType defaultDataVector)
    {
        size_t numNonZeros = 0;
        bool includesNonFloats = false;
        bool includesNonShorts = false;
        bool includesNonBytes = false;
        bool includesNonBinary = false;

        auto iter = defaultDataVector.GetIterator();
        while (iter.IsValid())
        {
            double value = iter.Get().value;

            ++numNonZeros;
            includesNonFloats |= DoesCastModifyValue<float>(value);
            includesNonShorts |= DoesCastModifyValue<short>(value);
            includesNonBytes |= DoesCastModifyValue<char>(value);
            includesNonBinary |= (value - 1.0 > APPROXIMATION_TOLERANCE || 1.0 - value > APPROXIMATION_TOLERANCE);

            iter.Next();
        }

        // dense 
        if (numNonZeros > SPARSE_THRESHOLD * defaultDataVector.Size())
        {
            if (includesNonFloats)
            {
                _pInternal = std::make_unique<DoubleDataVector>(std::move(defaultDataVector));
            }
            else if (includesNonShorts)
            {
                _pInternal = std::make_unique<FloatDataVector>(std::move(defaultDataVector));
            }
            else if (includesNonBytes)
            {
                _pInternal = std::make_unique<ShortDataVector>(std::move(defaultDataVector));
            }
            else
            {
                _pInternal = std::make_unique<ByteDataVector>(std::move(defaultDataVector));
            }
        }

        // sparse
        else
        {
            if (includesNonFloats)
            {
                _pInternal = std::make_unique<SparseDoubleDataVector>(std::move(defaultDataVector));
            }
            else if (includesNonShorts)
            {
                _pInternal = std::make_unique<SparseFloatDataVector>(std::move(defaultDataVector));
            }
            else if (includesNonBytes)
            {
                _pInternal = std::make_unique<SparseShortDataVector>(std::move(defaultDataVector));
            }
            else if(includesNonBinary)
            {
                _pInternal = std::make_unique<SparseByteDataVector>(std::move(defaultDataVector));
            }
            else
            {
                _pInternal = std::make_unique<SparseBinaryDataVector>(std::move(defaultDataVector));
            }
        }
    }
}
}