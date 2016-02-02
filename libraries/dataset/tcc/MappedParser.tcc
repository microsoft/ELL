// MappedParser.tcc

// utilities
#include "Parser.h"

namespace dataset
{
    template<typename InternalParserType>
    MappedParser<InternalParserType>::MappedParser(const InternalParserType& internalParser, const layers::Map& map, layers::CoordinateList outputCoordinates) :
        _internalParser(internalParser),
        _map(map),
        _outputCoordinates(outputCoordinates)
    {}

    template<typename InternalParserType>
    layers::Map::Iterator MappedParser<InternalParserType>::GetIterator(std::shared_ptr<const std::string> spExampleString, const char* pStr) const
    {
        auto internalIterator = _internalParser.GetIterator(spExampleString, pStr);
        return _map.Compute(internalIterator, _outputCoordinates);
    }
}
