// operators.h

#pragma once

#include "IMatrix.h"

#include <memory>
using std::shared_ptr;

#include <vector>
using std::vector;


namespace linear
{
    function<void(shared_ptr<vector<double>>)> operator*(shared_ptr<IMatrix> M, shared_ptr<vector<double>> x)
    {
        return[M, x](shared_ptr<vector<double>> y) {M->Gemv(*x.get(), *y.get());};
    }

    function<void(shared_ptr<vector<double>>)> operator*(shared_ptr<vector<double>> x, shared_ptr<IMatrix> M)
    {
        return[x, M](shared_ptr<vector<double>> y) {M->Gevm(*x.get(), *y.get());};
    }
}