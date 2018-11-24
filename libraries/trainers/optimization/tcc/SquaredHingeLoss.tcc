////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquaredHingeLoss.tcc (optimization)
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
        bool SquaredHingeLoss::VerifyOutput(OutputType output)
        {
            if (output == 1.0 || output == -1.0)
            {
                return true;
            }
            return false;
        }

        template <typename OutputType>
        double SquaredHingeLoss::Value(double prediction, OutputType output)
        {
            DEBUG_CHECK(VerifyOutput(output), "Squared Hinge Loss requires binary outputs");

            double margin = prediction * output;

            if (margin >= 1.0)
            {
                return 0.0;
            }
            double hinge = 1.0 - margin;
            return 0.5 * hinge * hinge;
        }

        template <typename OutputType>
        double SquaredHingeLoss::Derivative(double prediction, OutputType output)
        {
            DEBUG_CHECK(VerifyOutput(output), "Squared Hinge Loss requires binary outputs");

            double margin = prediction * output;

            if (margin >= 1.0)
            {
                return 0.0;
            }
            return -output * (1.0 - margin);
        }

        template <typename OutputType>
        double SquaredHingeLoss::Conjugate(double v, OutputType output)
        {
            DEBUG_CHECK(VerifyOutput(output), "Squared Hinge Loss requires binary outputs");

            double a = output * v;

            if (a <= 0)
            {
                return a + 0.5 * v * v;
            }
            return std::numeric_limits<double>::infinity();
        }

        template <typename OutputType>
        double SquaredHingeLoss::ConjugateProx(double theta, double z, OutputType output)
        {
            DEBUG_CHECK(VerifyOutput(output), "Squared Hinge Loss requires binary outputs");

            if (output * z <= theta)
            {
                return (z - theta * output) / (1 + theta);
            }
            return 0.0;
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell