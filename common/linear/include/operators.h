// operators.h

#pragma once

#include "IMatrix.h"
#include "SparseBinaryDatavector.h"
#include "IntegerList.h"
#include <functional>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;
using std::function;

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