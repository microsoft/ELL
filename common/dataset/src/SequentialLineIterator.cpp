// SequentialLineIterator.cpp

#include "SequentialLineIterator.h"

namespace dataset
{
	SequentialLineIterator::SequentialLineIterator(istream& is) : istream_iterator<DelimitedText<'\n'>>(is)
	{}

	bool SequentialLineIterator::IsValid() const
	{
		return *this != istream_iterator<DelimitedText<'\n'>>();
	}

	void SequentialLineIterator::Next()
	{
		++(*this);
	}

	const string& SequentialLineIterator::GetValue() const
	{
		return **this;
	}
}