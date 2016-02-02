// MappedParser.h

#pragma once

#include "types.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// stl
#include <string>
#include <memory>

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
        MappedParser(const InternalParserType& internalParser, const layers::Map& map, layers::CoordinateList outputCoordinates);

        /// \returns An iterator that parses the given std::string
        ///
        layers::Map::Iterator GetIterator(std::shared_ptr<const std::string> spExampleString, const char* pStr) const;

    private:
        InternalParserType _internalParser;
        layers::Map _map;
        layers::CoordinateList _outputCoordinates;
    };
}

#include "../tcc/MappedParser.tcc"
