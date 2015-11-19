// Scale.tcc

namespace mappings
{
    template <typename IndexValueIteratorType>
    Scale::Scale(IndexValueIteratorType begin, IndexValueIteratorType end) : Coordinatewise(begin, end, Scale::Multiply)
    {}
}