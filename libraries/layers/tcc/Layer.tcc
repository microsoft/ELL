// Layer.tcc

namespace layers
{
    template<typename IndexValueIteratorType, typename concept>
    void Layer::Set(IndexValueIteratorType indexValueIterator)
    {
        Clear();

        while(indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.GetValue();

            if(entry.index >= _output.size())
            {
                return;
            }

            _output[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }
}