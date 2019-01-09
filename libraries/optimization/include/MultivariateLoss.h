////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultivariateLoss.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Vector.h>

namespace ell
{
namespace optimization
{
    /// <summary> Adapter that extends a scalar loss class to vector inputs. </summary>
    template <typename LossType>
    class MultivariateLoss
    {
    public:
        /// <summary> Constructor. </summary>
        MultivariateLoss(LossType univariateLoss = {});

        /// <summary> Checks if an output is compatible with this loss. </summary>
        template <typename OutputElementType>
        bool VerifyOutput(math::ConstRowVectorReference<OutputElementType> output) const;

        /// <summary> Returns the loss of a vector prediction, given the true vector output. </summary>
        ///
        /// <param name="prediction"> The predicted output. </param>
        /// <param name="output"> The true output. </param>
        ///
        /// <returns> The loss. </returns>
        template <typename OutputElementType>
        double Value(math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const;

        /// <summary> Returns the loss gradient at a given vector point. </summary>
        ///
        /// <param name="prediction"> The predicted output. </param>
        /// <param name="output"> The true output. </param>
        ///
        /// <returns> The loss gradient. </returns>
        template <typename OutputElementType>
        math::RowVector<double> Derivative(math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const;

        /// <summary> Returns the value of the loss conjugate at a given vector point. </summary>
        ///
        /// <param name="dual"> The dual variable. </param>
        /// <param name="output"> The output. </param>
        ///
        /// <returns> Value of the loss conjugate. </returns>
        template <typename OutputElementType>
        double Conjugate(math::ConstRowVectorReference<double> dual, math::ConstRowVectorReference<OutputElementType> output) const;

        /// <summary>
        /// Returns the value of the proximal operator of the conjugate of the loss, which is
        ///
        ///     argmin_b {theta*g(b) + (1/2)*||b - a||_2^2}
        ///
        /// where ||x||_2 is the 2-norm and g() is the convex conjugate of f()
        /// </summary>
        ///
        /// <param name="sigma"> The sigma parameter. </param>
        /// <param name="prediction"> The prediction. </param>
        /// <param name="output"> The output. </param>
        ///
        /// <returns> Value of the loss conjugate prox. </returns>
        template <typename OutputElementType>
        math::RowVector<double> ConjugateProx(double sigma, math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const;

    private:
        LossType _univariateLoss;
    };
} // namespace optimization
} // namespace ell

#pragma region implementation

namespace ell
{
namespace optimization
{
    template <typename LossType>
    MultivariateLoss<LossType>::MultivariateLoss(LossType univariateLoss) :
        _univariateLoss(std::move(univariateLoss))
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
} // namespace optimization
} // namespace ell

#pragma endregion implementation
