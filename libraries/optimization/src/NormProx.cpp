////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NormProx.cpp (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NormProx.h"

#include <algorithm>
#include <numeric>

namespace ell
{
namespace optimization
{
    void L1Prox(math::ColumnVectorReference<double> v, double beta)
    {
        auto transformation = [beta](double x) {
            return x - std::copysign(std::min(beta, std::abs(x)), x);
        };
        v.Transform(transformation);
    }

    void LInfinityProx(math::ColumnVectorReference<double> v, std::vector<size_t>& scratch, double beta)
    {
        // special case: empty vector or beta=0
        if (v.Size() == 0 || beta == 0)
        {
            return;
        }

        // special case: small 1-norm, v goes to zero
        if (v.Norm1() <= beta)
        {
            v.Reset();
            return;
        }

        // special case: vector of size one
        if (v.Size() == 1)
        {
            if (v[0] > 0)
            {
                v[0] -= beta;
            }
            else
            {
                v[0] += beta;
            }
            return;
        }

        // general case:

        // first, make sure that the scratch space is the correct size
        if (scratch.size() != v.Size())
        {
            scratch.resize(v.Size());
        }

        // compute a descending absolute permutation
        std::vector<size_t>& order = scratch;
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](size_t lhs, size_t rhs) { return std::abs(v[lhs]) > std::abs(v[rhs]); });

        // find the number of coordinates that change
        size_t count = 1;
        double value = std::abs(v[order[1]]);
        double totalMovement = std::abs(v[order[0]]) - value;
        while (totalMovement < beta && count < v.Size() - 1)
        {
            ++count;
            double nextAbsValue = std::abs(v[order[count]]);
            totalMovement += (value - nextAbsValue) * count;
            value = nextAbsValue;
        }

        // find the value that the moving coordinates change to
        if (totalMovement <= beta)
        {
            ++count;
            value -= (beta - totalMovement) / count;
        }
        else
        {
            value += (totalMovement - beta) / count;
        }

        // change the v according to count and value
        for (size_t i = 0; i < count; ++i)
        {
            if (v[order[i]] > 0)
            {
                v[order[i]] = value;
            }
            else
            {
                v[order[i]] = -value;
            }
        }
    }
} // namespace optimization
} // namespace ell
