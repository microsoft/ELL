////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     L2Regularizer.tcc (optimization)
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
    double L2Regularizer::Value(const SolutionType& w)
    {
        return 0.5 * Norm2Squared(w);
    }

    template <typename SolutionType>
    double L2Regularizer::Conjugate(const SolutionType& v)
    {
        return 0.5 * Norm2Squared(v);
    }

    template <typename SolutionType>
    void L2Regularizer::ConjugateGradient(const SolutionType& v, SolutionType& w)
    {
        w = v;
    }
}
}
}