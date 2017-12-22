////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AutoDataVectorBase.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#define APPROXIMATION_TOLERANCE 1.0e-9
#define SPARSE_THRESHOLD 0.2

#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "SparseDataVector.h"

namespace ell
{
namespace data
{
    template <typename DefaultDataVectorType>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(DefaultDataVectorType&& vector)
    {
        FindBestRepresentation(std::move(vector));
    }

    template <typename DefaultDataVectorType>
    template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(IndexValueIteratorType indexValueIterator)
    {
        DefaultDataVectorType defaultDataVector(std::move(indexValueIterator));
        FindBestRepresentation(std::move(defaultDataVector));
    }

    template <typename DefaultDataVectorType>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(std::initializer_list<IndexValue> list)
    {
        DefaultDataVectorType defaultDataVector(std::move(list));
        FindBestRepresentation(std::move(defaultDataVector));
    }

    template <typename DefaultDataVectorType>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(std::initializer_list<double> list)
    {
        DefaultDataVectorType defaultDataVector(std::move(list));
        FindBestRepresentation(std::move(defaultDataVector));
    }

    template <typename DefaultDataVectorType>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(std::vector<IndexValue> vec)
    {
        DefaultDataVectorType defaultDataVector(std::move(vec));
        FindBestRepresentation(std::move(defaultDataVector));
    }

    template <typename DefaultDataVectorType>
    AutoDataVectorBase<DefaultDataVectorType>::AutoDataVectorBase(std::vector<double> vec)
    {
        DefaultDataVectorType defaultDataVector(std::move(vec));
        FindBestRepresentation(std::move(defaultDataVector));
    }

    template <typename DefaultDataVectorType>
    void AutoDataVectorBase<DefaultDataVectorType>::AppendElement(size_t /*index*/, double /*value*/)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Append element not supported for AutoDataVector");
    }

    template <typename DefaultDataVectorType>
    double AutoDataVectorBase<DefaultDataVectorType>::Dot(math::UnorientedConstVectorBase<double> vector) const
    {
        return _pInternal->Dot(vector);
    }

    template <typename DefaultDataVectorType>
    float AutoDataVectorBase<DefaultDataVectorType>::Dot(math::UnorientedConstVectorBase<float> vector) const
    {
        return _pInternal->Dot(vector);
    }

    template <typename DefaultDataVectorType>
    void AutoDataVectorBase<DefaultDataVectorType>::AddTo(math::RowVectorReference<double> vector) const
    {
        _pInternal->AddTo(vector);
    }

    template <typename DefaultDataVectorType>
    std::vector<double> AutoDataVectorBase<DefaultDataVectorType>::ToArray(size_t size) const
    {
        return _pInternal->ToArray(size);
    }

    template <typename DefaultDataVectorType>
    void AutoDataVectorBase<DefaultDataVectorType>::Print(std::ostream& os) const
    {
        _pInternal->Print(os);
    }

    template <typename DefaultDataVectorType>
    template <IterationPolicy policy, typename TransformationType>
    void AutoDataVectorBase<DefaultDataVectorType>::AddTransformedTo(math::RowVectorReference<double> vector, TransformationType transformation) const
    {
        _pInternal->AddTransformedTo<policy>(vector, transformation);
    }

    template <typename DefaultDataVectorType>
    template <typename ReturnType, typename... ArgTypes>
    ReturnType AutoDataVectorBase<DefaultDataVectorType>::CopyAs(ArgTypes... args) const
    {
        return _pInternal->CopyAs<ReturnType>(args...);
    }

    template <typename DefaultDataVectorType>
    template <IterationPolicy policy, typename ReturnType, typename... ArgTypes>
    ReturnType AutoDataVectorBase<DefaultDataVectorType>::TransformAs(ArgTypes... args) const
    {
        return _pInternal->TransformAs<policy, ReturnType>(args...);
    }

    template <typename TargetType>
    bool DoesCastModifyValue(double value)
    {
        double target = static_cast<double>(static_cast<TargetType>(value));
        return (target - value > APPROXIMATION_TOLERANCE) || (value - target > APPROXIMATION_TOLERANCE);
    }

    template <typename DefaultDataVectorType>
    void AutoDataVectorBase<DefaultDataVectorType>::FindBestRepresentation(DefaultDataVectorType defaultDataVector)
    {
        size_t numNonZeros = 0;
        bool includesNonFloats = false;
        bool includesNonShorts = false;
        bool includesNonBytes = false;
        bool includesNonBinary = false;

        auto iter = GetIterator<DefaultDataVectorType, IterationPolicy::skipZeros>(defaultDataVector);
        while (iter.IsValid())
        {
            double value = iter.Get().value;

            ++numNonZeros;
            includesNonFloats |= DoesCastModifyValue<float>(value);
            includesNonShorts |= DoesCastModifyValue<short>(value);
            includesNonBytes |= DoesCastModifyValue<char>(value);
            includesNonBinary |= (value != 1 && value != 0);

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
            else if (includesNonBinary)
            {
                SetInternal<SparseByteDataVector>(std::move(defaultDataVector));
            }
            else
            {
                SetInternal<SparseBinaryDataVector>(std::move(defaultDataVector));
            }
        }
    }

    template <typename DefaultDataVectorType>
    template <typename DataVectorType, utilities::IsDifferent<DataVectorType, DefaultDataVectorType> Concept>
    void AutoDataVectorBase<DefaultDataVectorType>::SetInternal(DefaultDataVectorType defaultDataVector)
    {
        _pInternal = std::make_unique<DataVectorType>(GetIterator<DefaultDataVectorType, IterationPolicy::skipZeros>(defaultDataVector));
    }

    template <typename IndexValueParsingIterator>
    AutoDataVector AutoDataVectorParser<IndexValueParsingIterator>::Parse(TextLine& textLine)
    {
        return AutoDataVector(IndexValueParsingIterator(textLine));
    }
}
}