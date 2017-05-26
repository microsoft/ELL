////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Normalizer.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
    template <data::IterationPolicy policy, typename TransformationType>
    inline Normalizer<policy, TransformationType>::Normalizer(TransformationType transformation)
        : _transformation(transformation)
    {
    }

    template <data::IterationPolicy policy, typename TransformationType>
    template <typename OutputDataVectorType, typename InputDataVectorType>
    OutputDataVectorType Normalizer<policy, TransformationType>::Compute(const InputDataVectorType& input) const
    {
        return data::TransformAs<InputDataVectorType, policy, OutputDataVectorType>(input, _transformation);
    }

    template <data::IterationPolicy policy, typename TransformationType>
    Normalizer<policy, TransformationType> MakeTransformationNormalizer(TransformationType transformation)
    {
        return Normalizer<policy, TransformationType>(transformation);
    }
}
}
