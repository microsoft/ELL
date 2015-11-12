/// IntegerList.cpp

#include "IntegerList.h"

#include <stdexcept>
using std::runtime_error;

namespace linear
{
	bool IntegerList::ConstIterator::IsValid() const
	{
		return _begin < _end;
	}

	void IntegerList::ConstIterator::Next()
	{
		++_begin;
	}

	uint IntegerList::ConstIterator::GetValue() const
	{
		return *_begin;
	}

	IntegerList::ConstIterator::ConstIterator(const vector_iterator& begin, const vector_iterator& end) : _begin(begin), _end(end)
	{}

	IntegerList::IntegerList()
	{}

	uint IntegerList::Size() const
	{
		return _list.size();
	}

	void IntegerList::Reserve(uint size)
	{
		_list.reserve(size);
	}

	uint IntegerList::Max() const
	{
		if(_list.size()==0)
		{
			throw runtime_error("Can't get max of empty list");
		}

		return _list[Size()-1];
	}

	void IntegerList::PushBack(uint value)
	{
		_list.push_back(value);
	}

	void IntegerList::Reset()
	{
		_list.resize(0);
	}

	IntegerList::ConstIterator IntegerList::GetConstIterator() const
	{
		return ConstIterator(_list.cbegin(), _list.cend());
	}

}