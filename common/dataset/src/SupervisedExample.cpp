// SupervisedExample.cpp

#include "SupervisedExample.h"

using std::move;

namespace dataset
{
	SupervisedExample::SupervisedExample(unique_ptr<IDataVector> instance, double label, double weight) : _up_instance(move(instance)), _label(label), _weight(weight)
	{}

	double SupervisedExample::GetWeight() const
	{
		return _weight;
	}

	double SupervisedExample::GetLabel() const
	{
		return _label;
	}
	
	IDataVector::type SupervisedExample::GetType() const
	{
		return type();
	}

	void SupervisedExample::Reset()
	{
		_up_instance->Reset();
	}

	void SupervisedExample::PushBack(uint index, double value)
	{
		_up_instance->PushBack(index, value);
	}

	//void SupervisedExample::foreach_nonzero(std::function<void(uint, double)> func, uint index_offset) const
	//{
	//	_up_instance->foreach_nonzero(func, index_offset);
	//}

	uint SupervisedExample::Size() const
	{
		return _up_instance->Size();
	}

	uint SupervisedExample::NumNonzeros() const
	{
		return _up_instance->NumNonzeros();
	}

	double SupervisedExample::norm2() const
	{
		return _up_instance->norm2();
	}

	void SupervisedExample::AddTo(double * p_other, double scalar) const
	{
		_up_instance->AddTo(p_other, scalar);
	}

	double SupervisedExample::Dot(const double * p_other) const
	{
		return _up_instance->Dot(p_other);
	}
}