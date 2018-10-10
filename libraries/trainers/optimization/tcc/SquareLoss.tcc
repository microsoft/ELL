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
    double SquareLoss::Conjugate(double dual, OutputType output)
    {
        return (0.5 * dual + output) * dual;
    }

    template <typename OutputType>
    double SquareLoss::ConjugateProx(double sigma, double dual, OutputType output)
    {
        return (dual - sigma * output) / (1 + sigma);
    }





}
}
}