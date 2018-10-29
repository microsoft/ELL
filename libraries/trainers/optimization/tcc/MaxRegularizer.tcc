////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MaxRegularizer.tcc (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename SolutionType>
    double MaxRegularizer::Value(const SolutionType& w) const
    {
        return 0.5 * Norm2Squared(w) + _beta * w.GetVector().NormInfinity(); // note: NormInfinity does not include the bias term
    }

    template <typename SolutionType>
    double MaxRegularizer::Conjugate(const SolutionType& v) const
    {
        SolutionType w = v;
        LInfinityProx(w.GetVector(), _scratch, _beta); // note: LInfinity term does not apply to the bias term
        double result = -_beta * w.GetVector().NormInfinity();
        w -= v;
        result += 0.5 * (Norm2Squared(v) - Norm2Squared(w));
        return result;
    }

    template <typename SolutionType>
    void MaxRegularizer::ConjugateGradient(const SolutionType& v, SolutionType& w) const
    {
        w = v;
        LInfinityProx(w.GetVector(), _scratch, _beta); // note: LInfinityProx does not apply to the bias term
    }
}
}
}