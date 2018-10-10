////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LogLoss.tcc (optimization)
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
    double LogLoss::Value(double prediction, OutputType output)
    {
        double margin = static_cast<double>(prediction * output);
        if (margin <= -18.0)
        {
            return -margin;
        }
        else
        {
            return std::log1p(std::exp(-margin));
        }
    }

    template <typename OutputType>
    double LogLoss::Derivative(double prediction, OutputType output)
    {
        double margin = static_cast<double>(prediction * output);

        if (margin <= 0.0)
        {
            auto exp_margin = std::exp(margin);
            return -output / (1 + exp_margin);
        }
        else
        {
            auto exp_neg_margin = std::exp(-margin);
            return -output * exp_neg_margin / (1 + exp_neg_margin);
        }
    }

    template <typename OutputType>
    double LogLoss::Conjugate(double dual, OutputType label)
    {
        const double conjugateBoundary = 1.0e-12;

        double x = dual * label;
        if (x < -1 || x > 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "dual times label must be in [-1,0]");
        }

        if (x >= -conjugateBoundary || x <= -1 + conjugateBoundary)
        {
            return 0.0;
        }
        else
        {
            return (-x) * std::log(-x) + (1.0 + x) * std::log1p(x);
        }
    }

    template <typename OutputType>
    double LogLoss::ConjugateProx(double sigma, double dual, OutputType output)
    {
        const double conjugateBoundary = 1.0e-12;
        const double conjugateProxdesiredDualityGap = 1.0e-6;
        const size_t conjugateProxMaxIterations = 20;

        double lowerBound = conjugateBoundary - 1.0;
        double upperBound = -conjugateBoundary;

        double x = dual * output;
        double b = std::fmin(upperBound, std::fmax(lowerBound, x));
        double f = 0, df = 0;
        for (size_t k = 0; k < conjugateProxMaxIterations; ++k)
        {
            f = b - x + sigma * log((1.0 + b) / (-b));
            if (fabs(f) <= conjugateProxdesiredDualityGap) break;
            df = 1.0 - sigma / (b * (1.0 + b));
            b -= f / df;
            b = fmin(upperBound, fmax(lowerBound, b));
        }
        return b * output;
    }
}
}
}