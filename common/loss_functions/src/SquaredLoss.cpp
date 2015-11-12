// SquaredLoss.cpp

#include "SquaredLoss.h"

#include <cmath>
using std::log;
using std::exp;

namespace loss_functions
{
	double SquaredLoss::Evaluate(double prediction, double label) const
	{
		double residual = prediction - label;
		return 0.5 * residual * residual;
	}

	double SquaredLoss::GetDerivative(double prediction, double label) const
	{
		double residual = prediction - label;
		return residual;
	}
}
