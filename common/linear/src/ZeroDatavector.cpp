// ZeroDatavector.cpp

#include "ZeroDatavector.h"
#include <stdexcept>

using std::runtime_error;

namespace linear
{
	ZeroDatavector::ZeroDatavector(const IDataVector& other) 
	{}

	IDataVector::type ZeroDatavector::GetType() const
	{
		return type::zero;
	}

	void ZeroDatavector::PushBack(uint index, double value)
	{
		if(value != 0)
		{
			throw runtime_error("Cannot set elements of a zero vector");
		}
	}

	void ZeroDatavector::Reset()
	{}

	//void ZeroDatavector::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const
	//{}

	uint ZeroDatavector::Size() const
	{
		return 0;
	}

	uint ZeroDatavector::NumNonzeros() const
	{
		return 0;
	}

	double ZeroDatavector::Norm2() const
	{
		return 0.0;
	}

	void ZeroDatavector::AddTo(double* p_other, double scalar) const
	{}

	double ZeroDatavector::Dot(const double* p_other) const
	{
		return 0.0;
	}
}