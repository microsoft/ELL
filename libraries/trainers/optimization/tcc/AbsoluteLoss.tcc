////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AbsoluteLoss.tcc (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cmath>

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename OutputType>
    double AbsoluteLoss::Value(double prediction, OutputType output)
    {
        return std::abs(prediction - output);
    }

    template <typename OutputType>
    double AbsoluteLoss::Derivative(double prediction, OutputType output)
    {
        if (prediction == output)
        {
            return 0.0;
        }
        if (prediction < output)
        {
            return -1.0;
        }
        return 1.0;
    }

    template <typename OutputType>
    double AbsoluteLoss::Conjugate(double v, OutputType output)
    {
        if (-1.0 <= v && v <= 1.0)
        {
            return output * v;
        }
        return std::numeric_limits<double>::infinity();
    }

    template <typename OutputType>
    double AbsoluteLoss::ConjugateProx(double theta, double z, OutputType output)
    {
        double a = z - theta * output;

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