////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HingeLoss.tcc (optimization)
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
    bool HingeLoss::VerifyOutput(OutputType output)
    { 
        if (output == 1.0 || output == -1.0)
        {
            return true;
        }
        return false; 
    }

    template <typename OutputType>
    double HingeLoss::Value(double prediction, OutputType output)
    {
        DEBUG_CHECK(VerifyOutput(output), "Hinge Loss requires binary outputs");

        double margin = prediction * output;

        if (margin >= 1.0)
        {
            return 0.0;
        }
        return 1.0 - margin;
    }

    template <typename OutputType>
    double HingeLoss::Derivative(double prediction, OutputType output)
    {
        DEBUG_CHECK(VerifyOutput(output), "Hinge Loss requires binary outputs");

        double margin = prediction * output;

        if (margin >= 1.0)
        {
            return 0.0;
        }
        return -output;
    }

    template <typename OutputType>
    double HingeLoss::Conjugate(double v, OutputType output)
    {
        DEBUG_CHECK(VerifyOutput(output), "Hinge Loss requires binary outputs");

        double a = output * v;

        if (-1.0 <= a && a <= 0.0)
        {
            return a;
        }
        return std::numeric_limits<double>::infinity();
    }

    template <typename OutputType>
    double HingeLoss::ConjugateProx(double theta, double z, OutputType output)
    {
        DEBUG_CHECK(VerifyOutput(output), "Hinge Loss requires binary outputs");

        double a = output * z;

        if (a < theta - 1.0)
        {
            return -output;
        }
        if (a <= theta)
        {
            return z - theta * output;
        }
        return 0.0;
    }
}
}
}