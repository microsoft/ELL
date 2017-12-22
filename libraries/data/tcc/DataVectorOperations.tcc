////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataVectorOperations.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace data
{
    template <typename DataVectorType, IsDataVector<DataVectorType> Concept>
    auto operator*(double scalar, const DataVectorType& vector)
    {
        return MakeTransformedDataVector<IterationPolicy::skipZeros>(vector, [scalar](IndexValue x) { return scalar*x.value; });
    }

    template <typename DataVectorType, IsDataVector<DataVectorType> Concept>
    auto operator*(const DataVectorType& vector, double scalar)
    {
        return scalar * vector;
    }

    template <typename ElementType>
    ElementType operator*(math::UnorientedConstVectorBase<ElementType> vector, const IDataVector& dataVector)
    {
        return dataVector.Dot(vector);
    }

    template <typename DataVectorType>
    auto Square(const DataVectorType& vector)
    {
        return MakeTransformedDataVector<IterationPolicy::skipZeros>(vector, [](IndexValue x) { return x.value*x.value; });
    }

    template <typename DataVectorType>
    auto Sqrt(const DataVectorType& vector)
    {
        return MakeTransformedDataVector<IterationPolicy::skipZeros>(vector, [](IndexValue x) { return std::sqrt(x.value); });
    }

    template <typename DataVectorType>
    auto Abs(const DataVectorType& vector)
    {
        return MakeTransformedDataVector<IterationPolicy::skipZeros>(vector, [](IndexValue x) { return std::abs(x.value); });
    }

    template <typename DataVectorType>
    auto ZeroIndicator(const DataVectorType& vector)
    {
        return MakeTransformedDataVector<IterationPolicy::all>(vector, [](IndexValue x) { return x.value == 0.0 ? 1.0 : 0.0; });
    }
}
}