////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquareLoss.tcc (optimization)
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
        double SquareLoss::Value(double prediction, OutputType output)
        {
            double residual = prediction - output;
            return 0.5 * residual * residual;
        }

        template <typename OutputType>
        double SquareLoss::Derivative(double prediction, OutputType output)
        {
            return prediction - output;
        }

        template <typename OutputType>
        double SquareLoss::Conjugate(double v, OutputType output)
        {
            return (0.5 * v + output) * v;
        }

        template <typename OutputType>
        double SquareLoss::ConjugateProx(double theta, double z, OutputType output)
        {
            return (z - theta * output) / (1 + theta);
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell