////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     MappedParser.tcc (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Parser.h"

namespace dataset
{
    template<typename InternalParserType>
    MappedParser<InternalParserType>::MappedParser(const InternalParserType& internalParser, const std::shared_ptr<layers::Map>& map, layers::CoordinateList outputCoordinates) :
        _internalParser(internalParser),
        _map(map),
        _outputCoordinates(outputCoordinates)
    {}

    template<typename InternalParserType>
    layers::Map::OutputIterator MappedParser<InternalParserType>::GetIterator(std::shared_ptr<const std::string> spExampleString, const char* pStr) const
    {
        auto internalIterator = _internalParser.GetIterator(spExampleString, pStr);
        return _map->Compute(internalIterator, _outputCoordinates);
    }
}
