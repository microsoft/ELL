// Map.tcc

namespace layers
{
    template<typename IndexValueIteratorType, typename concept>
    void Map::Compute(IndexValueIteratorType indexValueIterator)
    {
        // copy the values to layer zero
        _layers[0]->Set(indexValueIterator);

        // compute layers 1,2,... in order
        for(uint64 i = 1; i<_layers.size(); ++i)
        {
            _layers[i]->Compute(_layers);
        }
    }
}