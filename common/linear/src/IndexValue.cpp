#pragma once

#include "IndexValue.h"

namespace linear
{
	IndexValue::IndexValue(uint index, double value) : _index(index), _value(value)
	{}

	uint IndexValue::GetIndex() const
	{
		return _index;
	}

	/// \return The value
	///
	double IndexValue::GetValue() const
	{
		return _value;
	}


}
