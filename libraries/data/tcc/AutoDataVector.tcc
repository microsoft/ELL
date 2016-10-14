////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AutoDataVectorBase.tcc (data)
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
namespace data
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
    ReturnType AutoDataVectorBase<DefaultDataVectorType>::DeepCopyAs() const 
    {
        return _pInternal->DeepCopyAs<ReturnType>();
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
        if (numNonZeros > SPARSE_THRESHOLD * defaultDataVector.PrefixLength())
        {
            if (includesNonFloats)
            {
                SetInternal<DoubleDataVector>(std::move(defaultDataVector));
            }
            else if (includesNonShorts)
            {
                SetInternal<FloatDataVector>(std::move(defaultDataVector));
            }
            else if (includesNonBytes)
            {
                SetInternal<ShortDataVector>(std::move(defaultDataVector));
            }
            else
            {
                SetInternal<ByteDataVector>(std::move(defaultDataVector));
            }
        }

        // sparse
        else
        {
            if (includesNonFloats)
            {
                SetInternal<SparseDoubleDataVector>(std::move(defaultDataVector));
            }
            else if (includesNonShorts)
            {
                SetInternal<SparseFloatDataVector>(std::move(defaultDataVector));
            }
            else if (includesNonBytes)
            {
                SetInternal<SparseShortDataVector>(std::move(defaultDataVector));
            }
            else if(includesNonBinary)
            {
                SetInternal<SparseByteDataVector>(std::move(defaultDataVector));
            }
            else
            {
                SetInternal<SparseBinaryDataVector>(std::move(defaultDataVector));
            }
        }
    }

    template<typename DefaultDataVectorType>
    template<typename DataVectorType, utilities::IsDifferent<DataVectorType, DefaultDataVectorType> Concept>
    void AutoDataVectorBase<DefaultDataVectorType>::SetInternal(DefaultDataVectorType defaultDataVector)
    {
        _pInternal = std::make_unique<DataVectorType>(defaultDataVector.GetIterator());
    }

    template<typename DefaultDataVectorType>
    template<typename DataVectorType, utilities::IsSame<DataVectorType, DefaultDataVectorType> Concept>
    void AutoDataVectorBase<DefaultDataVectorType>::SetInternal(DefaultDataVectorType defaultDataVector)
    {
        _pInternal = std::make_unique<DefaultDataVectorType>(std::move(defaultDataVector));
    }
}
}