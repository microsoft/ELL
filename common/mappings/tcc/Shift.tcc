// Shift.tcc

namespace mappings
{
    template <typename IndexValueIteratorType>
    Shift::Shift(IndexValueIteratorType begin, IndexValueIteratorType end) : Coordinatewise(begin, end, Shift::Add)
    {}
}