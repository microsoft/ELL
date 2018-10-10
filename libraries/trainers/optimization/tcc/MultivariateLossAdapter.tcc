////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultivariateLossAdapter.tcc (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename LossType>
    template <typename OutputElementType>
    double MultivariateLossAdapter<LossType>::Value(math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const
    {
        double result = 0;
        for (size_t i = 0; i < prediction.Size(); ++i)
        {
            result += LossType::Value(prediction[i], output[i]);
        }
        return result;
    }

    template <typename LossType>
    template <typename OutputElementType>
    math::RowVector<double> MultivariateLossAdapter<LossType>::Derivative(math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const
    {
        math::RowVector<double> result(prediction.Size());
        for (size_t i = 0; i < prediction.Size(); ++i)
        {
            result[i] = LossType::Derivative(prediction[i], output[i]);
        }
        return result;
    }

    template <typename LossType>
    template <typename OutputElementType>
    double MultivariateLossAdapter<LossType>::Conjugate(math::ConstRowVectorReference<double> dual, math::ConstRowVectorReference<OutputElementType> output) const
    {
        double result = 0;
        for (size_t i = 0; i < dual.Size(); ++i)
        {
            result += LossType::Conjugate(dual[i], output[i]);
        }
        return result;
    }

    template <typename LossType>
    template <typename OutputElementType>
    math::RowVector<double> MultivariateLossAdapter<LossType>::ConjugateProx(double sigma, math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const
    {
        math::RowVector<double> result(prediction.Size());
        for (size_t i = 0; i < prediction.Size(); ++i)
        {
            result[i] = LossType::ConjugateProx(sigma, prediction[i], output[i]);
        }
        return result;
    }
}
}
}