#include "..\..\linear\include\IndexValue.h"
// IndexValue.cpp

#include "IndexValue.h"

namespace mappings
{
	IndexValue::IndexValue()
	{}

	IndexValue::IndexValue(int index, double value) : _index(index), _value(value)
	{}

	int IndexValue::GetIndex() const
	{
		return _index;
	}

	double IndexValue::GetValue() const
	{
		return _value;
	}

	void IndexValue::Serialize(JsonSerializer& js) const
	{
		js.write("index", _index);
		js.write("value", _value);
	}

	void IndexValue::Deserialize(JsonSerializer& js)
	{
		js.read("index", _index);
		js.read("value", _value);
	}
}