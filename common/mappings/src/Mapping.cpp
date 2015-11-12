// Mapping.cpp

#include "Mapping.h"

namespace mappings
{
	void Mapping::apply(const vector<double>& input, vector<double>& output) const
	{
		apply(&input[0], &output[0]);
	}

	Mapping::types Mapping::GetType() const
	{
		return _type;
	}

	void Mapping::SerializeHeader(JsonSerializer& js, int version) const
	{
		// to make the file more readable
		const string type_names[] = { "Constant", "Scale", "Shift", "Sum", "DecisionTreePath", "Row", "Column" };
		js.write("_type", type_names[(int)_type]);
		js.write("_version", version);
	}
}