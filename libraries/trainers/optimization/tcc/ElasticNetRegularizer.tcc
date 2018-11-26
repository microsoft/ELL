////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ElasticNetRegularizer.tcc (optimization)
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
        double ElasticNetRegularizer::Value(const SolutionType& w) const
        {
            return 0.5 * Norm2Squared(w) + _beta * w.GetVector().Norm1(); // note: Norm1 does not include the bias term
        }

        template <typename SolutionType>
        double ElasticNetRegularizer::Conjugate(const SolutionType& v) const
        {
            SolutionType w = v;
            L1Prox(w.GetVector(), _beta); // note: L1 term does not apply to the bias term
            double result = -_beta * w.GetVector().Norm1();
            w -= v;
            result += 0.5 * (Norm2Squared(v) - Norm2Squared(w));
            return result;
        }

        template <typename SolutionType>
        void ElasticNetRegularizer::ConjugateGradient(const SolutionType& v, SolutionType& w) const
        {
            w = v;
            L1Prox(w.GetVector(), _beta); // note: L1Prox does not apply to the bias term
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell
