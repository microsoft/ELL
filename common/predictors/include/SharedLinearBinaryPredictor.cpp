#include "SharedLinearBinaryPredictor.h"

predictors::SharedLinearBinaryPredictor::BiasedVector::BiasedVector(uint dim) : w(dim), b(0.0)
{}

predictors::SharedLinearBinaryPredictor::SharedLinearBinaryPredictor(uint dim) 
{
	_sp_predictor = make_shared<BiasedVector>(dim);
}

DoubleVector & predictors::SharedLinearBinaryPredictor::GetVector()
{
	return _sp_predictor->w;
}

const DoubleVector & predictors::SharedLinearBinaryPredictor::GetVector() const
{
	return _sp_predictor->w;
}

double & predictors::SharedLinearBinaryPredictor::GetBias()
{
	return _sp_predictor->b;
}

double predictors::SharedLinearBinaryPredictor::GetBias() const
{
	return _sp_predictor->b;
}
