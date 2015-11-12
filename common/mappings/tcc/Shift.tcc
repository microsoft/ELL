// Shift.tcc

namespace mappings
{
	template <typename IndexValueIterator>
	Shift::Shift(IndexValueIterator begin, IndexValueIterator end) : Coordinatewise(begin, end, Shift::Add)
	{}
}