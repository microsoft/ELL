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
    Map::Iterator Map::Compute(IndexValueIteratorType IndexValueIterator, const vector<Coordinate>& outputCoordinates) const
    {
        // allocate memory to store the output of the map calculation
        auto outputs = AllocateOutputs();
        
        // set the input // TODO - move this functionality into RealArray/DoubleArray (possibly move those into utilities project)
        uint64 inputSize = _layers[0]->Size();
        while (IndexValueIterator.IsValid())
        {
            IndexValue IndexValue = IndexValueIterator.Get();
            if (IndexValue.index >= inputSize) break;
            (*outputs)[0][IndexValue.index] = IndexValue.value;
            IndexValueIterator.Next();
        }

        // compute layers 1,2,... in order
        for(uint64 i = 1; i<_layers.size(); ++i)
        {
            _layers[i]->Compute(i, *outputs);
        }
        
        return Iterator(outputs, outputCoordinates);
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