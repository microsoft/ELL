// MappedParser.h

#pragma once

#include "types.h"

// layers
#include "Map.h"
using layers::Map;

// stl
#include <string>
using std::string;

#include <memory>
using std::shared_ptr;

namespace dataset
{
    /// Wraps any other parser and passes it through a map
    ///
    template<typename InternalParserType>
    class MappedParser
    {
    public:
        /// Ctor
        ///
        MappedParser(const InternalParserType& internalParser, const Map& map, shared_ptr<vector<Coordinate>> spOutputCoordinates);

        /// \returns An iterator that parses the given string
        ///
        Map::Iterator GetIterator(shared_ptr<const string> spExampleString, const char* pStr) const;

    private:
        InternalParserType _internalParser;
        Map _map;
        shared_ptr<vector<Coordinate>> _spOutputCoordinates;
    };
}

#include "../tcc/MappedParser.tcc"
