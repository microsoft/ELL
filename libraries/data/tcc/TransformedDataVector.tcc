////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformedDataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace data
{
    template <IterationPolicy policy, typename DataVectorType, typename TransformationType>
    TransformedDataVector<policy, DataVectorType, TransformationType>::TransformedDataVector(const DataVectorType& dataVector, TransformationType transformation)
        : _dataVector(dataVector), _transformation(transformation)
    {
    }

    template <IterationPolicy policy, typename DataVectorType, typename TransformationType>
    TransformedDataVector<policy, DataVectorType, TransformationType> MakeTransformedDataVector(const DataVectorType& dataVector, TransformationType transformation)
    {
        return TransformedDataVector<policy, DataVectorType, TransformationType>(dataVector, transformation);
    }

    template <IterationPolicy policy, typename DataVectorType, typename TransformationType>
    void operator+=(math::RowVectorReference<double> vector, const TransformedDataVector<policy, DataVectorType, TransformationType>& transformedDataVector)
    {
        AddTransformedTo<DataVectorType, policy>(transformedDataVector.GetDataVector(), vector, transformedDataVector.GetTransformation());
    }
}
}