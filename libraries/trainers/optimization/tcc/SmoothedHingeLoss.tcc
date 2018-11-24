////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SmoothedHingeLoss.tcc (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Common.h"

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename OutputType>
        bool SmoothedHingeLoss::VerifyOutput(OutputType output)
        {
            if (output == 1.0 || output == -1.0)
            {
                return true;
            }
            return false;
        }

        template <typename OutputType>
        double SmoothedHingeLoss::Value(double prediction, OutputType output) const
        {
            DEBUG_CHECK(VerifyOutput(output), "Smoothed Hinge Loss requires binary outputs");

            double margin = prediction * output;
            if (margin >= 1.0)
            {
                return 0.0;
            }
            if (margin >= 1.0 - _gamma)
            {
                double residual = (prediction - output);
                return 0.5 / _gamma * residual * residual;
            }
            return 1.0 - margin - 0.5 * _gamma;
        }

        template <typename OutputType>
        double SmoothedHingeLoss::Derivative(double prediction, OutputType output) const
        {
            DEBUG_CHECK(VerifyOutput(output), "Smoothed Hinge Loss requires binary outputs");

            double margin = prediction * output;
            if (margin >= 1.0)
            {
                return 0.0;
            }
            if (margin >= 1.0 - _gamma)
            {
                return (prediction - output) / _gamma;
            }
            return -output;
        }

        template <typename OutputType>
        double SmoothedHingeLoss::Conjugate(double v, OutputType output) const
        {
            DEBUG_CHECK(VerifyOutput(output), "Smoothed Hinge Loss requires binary outputs");

            double a = output * v;

            if (-1.0 <= a && a <= 0.0)
            {
                return a + 0.5 * _gamma * v * v;
            }
            return std::numeric_limits<double>::infinity();
        }

        template <typename OutputType>
        double SmoothedHingeLoss::ConjugateProx(double theta, double z, OutputType output) const
        {
            DEBUG_CHECK(VerifyOutput(output), "Smoothed Hinge Loss requires binary outputs");

            double a = output * z;

            if (a < (1.0 - _gamma) * theta - 1.0)
            {
                return -output;
            }
            if (a <= theta)
            {
                return (z - theta * output) / (1 + theta * _gamma);
            }
            return 0.0;
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell