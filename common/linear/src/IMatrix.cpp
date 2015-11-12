// IMatrix.cpp

#include "IMatrix.h"

#include <cassert>

namespace linear
{
    void IMatrix::Gemv(const vector<double>& x, vector<double>& y, double alpha, double beta) const
    {
        Gemv(&x[0], &y[0], alpha, beta);
    }

    void IMatrix::Gevm(const vector<double>& x, vector<double>& y, double alpha, double beta) const
    {
        Gevm(&x[0], &y[0], alpha, beta);
    }
}