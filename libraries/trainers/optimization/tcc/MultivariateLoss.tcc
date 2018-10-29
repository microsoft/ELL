////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultivariateLoss.tcc (optimization)
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
    MultivariateLoss<LossType>::MultivariateLoss(LossType univariateLoss) : _univariateLoss(std::move(univariateLoss))
    {}

    template <typename LossType>
    template <typename OutputElementType>
    bool MultivariateLoss<LossType>::VerifyOutput(math::ConstRowVectorReference<OutputElementType> output) const
    {
        for (size_t i = 0; i < output.Size(); ++i)
        {
            if (!_univariateLoss.VerifyOutput(output[i]))
            {
                return false;
            }
        }
        return true;
    }

    template <typename LossType>
    template <typename OutputElementType>
    double MultivariateLoss<LossType>::Value(math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const
    {
        double result = 0;
        for (size_t i = 0; i < prediction.Size(); ++i)
        {
            result += _univariateLoss.Value(prediction[i], output[i]);
        }
        return result;
    }

    template <typename LossType>
    template <typename OutputElementType>
    math::RowVector<double> MultivariateLoss<LossType>::Derivative(math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const
    {
        math::RowVector<double> result(prediction.Size());
        for (size_t i = 0; i < prediction.Size(); ++i)
        {
            result[i] = _univariateLoss.Derivative(prediction[i], output[i]);
        }
        return result;
    }

    template <typename LossType>
    template <typename OutputElementType>
    double MultivariateLoss<LossType>::Conjugate(math::ConstRowVectorReference<double> dual, math::ConstRowVectorReference<OutputElementType> output) const
    {
        double result = 0;
        for (size_t i = 0; i < dual.Size(); ++i)
        {
            result += _univariateLoss.Conjugate(dual[i], output[i]);
        }
        return result;
    }

    template <typename LossType>
    template <typename OutputElementType>
    math::RowVector<double> MultivariateLoss<LossType>::ConjugateProx(double sigma, math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const
    {
        math::RowVector<double> result(prediction.Size());
        for (size_t i = 0; i < prediction.Size(); ++i)
        {
            result[i] = _univariateLoss.ConjugateProx(sigma, prediction[i], output[i]);
        }
        return result;
    }
}
}
}