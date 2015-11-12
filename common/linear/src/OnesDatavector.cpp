// OnesDatavector.cpp

#include "OnesDatavector.h"
#include <stdexcept>
#include <cmath>
#include <cassert>

using std::runtime_error;
using std::fabs;
using std::move;

namespace linear
{
	OnesDatavector::OnesDatavector(uint dim) : _dim(dim)
	{}

	OnesDatavector::OnesDatavector(const IDataVector& other) : _dim(other.Size())
	{}

	IDataVector::type OnesDatavector::GetType() const
	{
		return type::ones;
	}

	void OnesDatavector::PushBack(uint index, double value)
	{
		if(value != 1 || index != _dim)
		{
			throw runtime_error("Cannot set elements of a ones vector to non-one values");
		}
		++_dim;
	}

	void OnesDatavector::Reset()
	{
		_dim = 0;
	}

	//void OnesDatavector::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const
	//{
	//	for(uint i=0; i<_dim; ++i)
	//	{
	//		func(index_offset+i, 1.0);
	//	}
	//}

	uint OnesDatavector::Size() const
	{
		return _dim;
	}

	uint OnesDatavector::NumNonzeros() const
	{
		return _dim;
	}

	double OnesDatavector::Norm2() const
	{
		return (double)_dim;
	}

	void OnesDatavector::AddTo(double* p_other, double scalar) const
	{
		for(uint i = 0; i<_dim; ++i)
		{
			p_other[i] += (double)scalar;
		}
	}

	double OnesDatavector::Dot(const double* p_other) const
	{
		double result = 0.0;
		for(uint i = 0; i<_dim; ++i)
		{
			result += p_other[i];
		}

		return result;
	}
}