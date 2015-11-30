// Map.tcc

#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include <string>
using std::string;
using std::istreambuf_iterator;

#include <memory>
using std::make_shared;

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

    template<typename MapType>
    shared_ptr<MapType> Map::Deserialize(istream& is)
    {
        // parse stream contents
        auto str = string(istreambuf_iterator<char>(is), istreambuf_iterator<char>());
        JsonSerializer reader;
        reader.FromString(str);

        // read Map
        auto map = make_shared<MapType>();
        reader.Read("Base", *map);

        return map;
    }

}