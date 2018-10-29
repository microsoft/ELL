////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HuberLoss.tcc (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename OutputType>
    double HuberLoss::Value(double prediction, OutputType output) const
    {
        double residual = prediction - output;

        if (residual >= -_gamma && residual <= _gamma)
        {
            return 0.5 / _gamma * residual * residual;
        }
        return std::abs(residual) - 0.5 * _gamma;
    }

    template <typename OutputType>
    double HuberLoss::Derivative(double prediction, OutputType output) const
    {
        double residual = prediction - output;

        if (residual >= -_gamma && residual <= _gamma)
        {
            return residual / _gamma;
        }
        if (residual > 0)
        {
            return 1.0;
        }
        return -1.0;
    }

    template <typename OutputType>
    double HuberLoss::Conjugate(double v, OutputType output) const
    {
        if (-1.0 <= v && v <= 1.0)
        {
            return output * v + 0.5 * _gamma * v * v;
        }
        return std::numeric_limits<double>::infinity();
    }

    template <typename OutputType>
    double HuberLoss::ConjugateProx(double theta, double z, OutputType output) const
    {
        double a = (z - theta * output) / (1 + theta * _gamma);

        if (a <= -1.0)
        {
            return -1.0;
        }
        if (a >= 1.0)
        {
            return 1.0;
        }
        return a;
    }
}
}
}