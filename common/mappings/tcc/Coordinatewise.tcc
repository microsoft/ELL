// Coordinatewise.tcc

namespace mappings
{

    template <typename IndexValueIterator>
    Coordinatewise::Coordinatewise(IndexValueIterator begin, IndexValueIterator end, function<double(double, double)> func) : _func(func)
    {
        while(begin != end)
        {
            _index_values.emplace_back(begin->GetIndex(), begin->GetValue());
            ++begin;
        }


    }
}