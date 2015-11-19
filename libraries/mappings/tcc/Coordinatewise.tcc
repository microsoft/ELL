// Coordinatewise.tcc

namespace mappings
{
   
    template<typename IndexValueIteratorType, typename concept>
    Coordinatewise::Coordinatewise(IndexValueIteratorType indexValueIterator, function<double(double, double)> func) : _func(func)
    {
        while(indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            _indexValues.push_back(entry);
            indexValueIterator.Next();
        }
    }
}