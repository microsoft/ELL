// Scale.tcc

namespace mappings
{
    template <typename IndexValueIteratorType>
    Scale::Scale(IndexValueIteratorType indexValueIterator) : Coordinatewise(indexValueIterator, Scale::Multiply)
    {}
}