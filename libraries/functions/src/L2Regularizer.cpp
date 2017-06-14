////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     L2Regularizer.cpp (functions)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "L2Regularizer.h"

namespace ell
{
namespace functions
{
    double L2Regularizer::operator()(math::ColumnConstVectorReference<double> w, double b) const
    {
        return 0.5 * (w.Norm2Squared() + b*b);
    }

    double L2Regularizer::Conjugate(math::ColumnConstVectorReference<double> v, double d) const
    {
        return (*this)(v, d);
    }

    void L2Regularizer::ConjugateGradient(math::ColumnConstVectorReference<double> v, math::ColumnVectorReference<double> w) const
    {
        w.CopyFrom(v);
    }

    void L2Regularizer::ConjugateGradient(math::ColumnConstVectorReference<double> v, double d, math::ColumnVectorReference<double> w, double& b) const
    {
        w.CopyFrom(v);
        b = d;
    }
}
}