// MappedParser.tcc

// utilities
#include "Parser.h"
using utilities::Parser;

#include <memory>
using std::move;

#include <stdexcept>
using std::runtime_error;

namespace dataset
{
    template<typename InternalParserType>
    MappedParser<InternalParserType>::MappedParser(const InternalParserType& internalParser, const Map& map, CoordinateList outputCoordinates) :
        _internalParser(internalParser),
        _map(map),
        _outputCoordinates(outputCoordinates)
    {}

    template<typename InternalParserType>
    Map::Iterator MappedParser<InternalParserType>::GetIterator(shared_ptr<const string> spExampleString, const char* pStr) const
    {
        auto internalIterator = _internalParser.GetIterator(spExampleString, pStr);
        return _map.Compute(internalIterator, _outputCoordinates);
    }
}
