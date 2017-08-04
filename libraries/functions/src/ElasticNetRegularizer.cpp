////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ElasticNetRegularizer.cpp (functions)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ElasticNetRegularizer.h"

// stl
#include <cmath>

namespace ell
{
namespace functions
{
    ElasticNetRegularizer::ElasticNetRegularizer(double ratioL1L2) : _ratioL1L2(ratioL1L2)
    {
    }

    double ElasticNetRegularizer::operator()(math::ConstColumnVectorReference<double> v, double b) const
    {
        return 0.5 * (v.Norm2Squared() + b*b) + _ratioL1L2 * (v.Norm1() + std::abs(b));
    }

    double ElasticNetRegularizer::Conjugate(math::ConstColumnVectorReference<double> v, double d) const
    {
        double dot = 0;
        double norm2Squared = 0;
        double norm1 = 0;

        // handle bias term
        double b = d - _ratioL1L2;
        if (b > 0)
        {
            dot = d * b;
            norm2Squared = b * b;
            norm1 = b;
        }
        else
        {
            b = d + _ratioL1L2;
            if (b < 0)
            {
                dot += d * b;
                norm2Squared += b * b;
                norm1 -= b;
            }
        }

        // handle vector
        for (size_t j = 0; j < v.Size(); ++j)
        {
            double w = v[j] - _ratioL1L2;
            if (w > 0)
            {
                dot += v[j] * w;
                norm2Squared += w*w;
                norm1 += w;
                continue;
            }

            w = v[j] + _ratioL1L2;
            if (w < 0)
            {
                dot += v[j] * w;
                norm2Squared += w*w;
                norm1 -= w;
            }
        }

        return dot - (0.5 * norm2Squared + _ratioL1L2 * norm1);
    }

    void ElasticNetRegularizer::ConjugateGradient(math::ConstColumnVectorReference<double> v, math::ColumnVectorReference<double> w) const
    {
        for (size_t j = 0; j < v.Size(); ++j)
        {
            double z = v[j] - _ratioL1L2;
            if (z > 0)
            {
                w[j] = z;
                continue;
            }

            z = v[j] + _ratioL1L2;
            if (z < 0)
            {
                w[j] = z;
            }

            w[j] = 0;
        }
    }

    void ElasticNetRegularizer::ConjugateGradient(math::ConstColumnVectorReference<double> v, double d, math::ColumnVectorReference<double> w, double& b) const
    {
        ConjugateGradient(v, w);
        b = d - _ratioL1L2;
        if (b < 0)
        {
            b = d + _ratioL1L2;
            if (b > 0)
            {
                b = 0;
            }
        }
    }
}
}
