// MappedParser.tcc

// utilities
#include "Parser.h"
using utilities::Parser;

#include <memory>
using std::move;

#include <stdexcept>
#include "..\include\MappedParser.h"
using std::runtime_error;

namespace dataset
{
    template<typename InternalParserType>
    MappedParser<InternalParserType>::MappedParser(const InternalParserType& internalParser, const Map& map, shared_ptr<vector<Coordinate>> spOutputCoordinates) :
        _internalParser(internalParser),
        _map(map),
        _spOutputCoordinates(spOutputCoordinates)
    {}

    template<typename InternalParserType>
    Map::Iterator MappedParser<InternalParserType>::GetIterator(shared_ptr<const string> spExampleString, const char* pStr) const
    {
        auto internalIterator = _internalParser.GetIterator(spExampleString, pStr);
        return _map.Compute(internalIterator, _spOutputCoordinates);
    }
}