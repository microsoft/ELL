////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     MappedParser.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Parser.h"

namespace dataset
{
    template<typename InternalParserType>
    MappedParser<InternalParserType>::MappedParser(InternalParserType internalParser, layers::Map map) :
        _internalParser(std::move(internalParser)),
        _map(std::move(map))
    {}

    template<typename InternalParserType>
    layers::Map::OutputIterator MappedParser<InternalParserType>::GetIterator(std::shared_ptr<const std::string> spExampleString, const char* pStr) const
    {
        auto internalIterator = _internalParser.GetIterator(spExampleString, pStr);
        return _map.Compute(internalIterator);
    }
}
