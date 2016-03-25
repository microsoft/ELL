////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     IMatrix.cpp (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IMatrix.h"

namespace linear
{
    void IMatrix::Gemv(const std::vector<double>& x, std::vector<double>& y, double alpha, double beta) const
    {
        Gemv(&x[0], &y[0], alpha, beta);
    }

    void IMatrix::Gevm(const std::vector<double>& x, std::vector<double>& y, double alpha, double beta) const
    {
        Gevm(&x[0], &y[0], alpha, beta);
    }
}
