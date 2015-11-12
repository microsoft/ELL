// Scale.tcc

namespace mappings
{
    template <typename IndexValueIterator>
    Scale::Scale(IndexValueIterator begin, IndexValueIterator end) : Coordinatewise(begin, end, Scale::Multiply)
    {}
}