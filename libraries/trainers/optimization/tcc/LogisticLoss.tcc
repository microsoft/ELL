////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LogisticLoss.tcc (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Common.h"

// stl
#include <cmath>

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename OutputType>
    bool LogisticLoss::VerifyOutput(OutputType output)
    {
        if (output == 1.0 || output == -1.0)
        {
            return true;
        }
        return false;
    }

    template <typename OutputType>
    double LogisticLoss::Value(double prediction, OutputType output)
    {
        DEBUG_CHECK(VerifyOutput(output), "Logistic Loss requires binary outputs");

        const double exponentLimit = 18.0;
        
        double margin = prediction * output;
        
        if (margin <= -exponentLimit)
        {
            return -margin;
        }
        return std::log1p(std::exp(-margin));
    }

    template <typename OutputType>
    double LogisticLoss::Derivative(double prediction, OutputType output)
    {
        DEBUG_CHECK(VerifyOutput(output), "Logistic Loss requires binary outputs");

        double margin = static_cast<double>(prediction * output);

        if (margin <= 0.0)
        {
            return -output / (1 + std::exp(margin));
        }
        auto expNegMargin = std::exp(-margin);
        return -output * expNegMargin / (1 + expNegMargin);
    }

    template <typename OutputType>
    double LogisticLoss::Conjugate(double v, OutputType output)
    {
        DEBUG_CHECK(VerifyOutput(output), "Logistic Loss requires binary outputs");

        const double conjugateBoundary = 1.0e-12;
        double a = output * v;

        if (a < -1.0 || a > 0.0)
        {
            return std::numeric_limits<double>::infinity();
        }
        if (a <= conjugateBoundary - 1.0 || -conjugateBoundary <= a)
        {
            return 0.0;
        }
        return (1.0 + a) * std::log1p(a) + (-a) * std::log(-a);
    }

    template <typename OutputType>
    double LogisticLoss::ConjugateProx(double theta, double z, OutputType output)
    {
        DEBUG_CHECK(VerifyOutput(output), "Logistic Loss requires binary outputs");

        const double conjugateBoundary = 1.0e-12;
        const double conjugateProxDesiredDualityGap = 1.0e-6;
        const size_t conjugateProxMaxIterations = 20;

        double lowerBound = conjugateBoundary - 1.0;
        double upperBound = -conjugateBoundary;

        double a = output * z;
        double b = std::min(upperBound, std::max(lowerBound, a));
        double f = 0, df = 0;
        for (size_t k = 0; k < conjugateProxMaxIterations; ++k)
        {
            f = b - a + theta * log((1.0 + b) / (-b));
            if (std::abs(f) <= conjugateProxDesiredDualityGap) break;
            df = 1.0 - theta / (b * (1.0 + b));
            b -= f / df;
            b = std::min(upperBound, std::max(lowerBound, b));
        }
        return b * output;
    }
}
}
}