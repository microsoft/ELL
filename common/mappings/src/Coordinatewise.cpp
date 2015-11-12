// Coordinatewise.cpp

#include "Coordinatewise.h"
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::to_string;

namespace mappings
{
	Coordinatewise::Coordinatewise(function<double(double, double)> func) : _func(func)
	{}

	Coordinatewise::index_value_iterator Coordinatewise::begin() const
	{
		return _index_values.cbegin();
	}

	Coordinatewise::index_value_iterator Coordinatewise::end() const
	{
		return _index_values.cend();
	}

	void Coordinatewise::apply(const double* input, double* output) const
	{
		for(size_t i = 0; i<_index_values.size(); ++i)
		{
			int index = _index_values[i].GetIndex();
			double value = _index_values[i].GetValue();
			output[i] = _func(input[index], value);
		}
	}

	int Coordinatewise::GetMinInputDim() const
	{
		int min_input_dim = 0;
		for(size_t i = 0; i<_index_values.size(); ++i)
		{
			if(_index_values[i].GetIndex() >= min_input_dim)
			{
				min_input_dim = _index_values[i].GetIndex() + 1;
			}
		}
		return min_input_dim;
	}

	int Coordinatewise::GetOutputDim() const
	{
		return (int)_index_values.size();
	}

	void Coordinatewise::Serialize(JsonSerializer& js) const
	{
		// version 1
		Mapping::SerializeHeader(js, 1);
		js.write("index_values", _index_values);
	}

	void Coordinatewise::Deserialize(JsonSerializer& js, int version)
	{
		if(version == 1)
		{
			js.read("index_values", _index_values);
		}
		else
		{
			throw runtime_error("unsupported version: " + to_string(version));
		}
	}
}


