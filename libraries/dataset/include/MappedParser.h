////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     MappedParser.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// layers
#include "Map.h"
#include "Coordinate.h"

// stl
#include <string>
#include <memory>

namespace dataset
{
    /// <summary> Wraps any other parser and passes it through a map. </summary>
    ///
    /// <typeparam name="ternalParserType"> Type of the internal parser type. </typeparam>
    template<typename InternalParserType>
    class MappedParser
    {
    public:
        /// <summary> Constructs an instance of MappedParser. </summary>
        ///
        /// <param name="internalParser"> The internal parser. </param>
        /// <param name="map"> The map. </param>
        MappedParser(InternalParserType internalParser, layers::Map map);

        /// <summary> Gets an iterator that parses the given std::string. </summary>
        ///
        /// <param name="spExampleString"> The sp example string. </param>
        /// <param name="pStr"> The string. </param>
        ///
        /// <returns> The iterator. </returns>
        layers::Map::OutputIterator GetIterator(std::shared_ptr<const std::string> spExampleString, const char* pStr) const;

    private:
        const InternalParserType _internalParser;
        const layers::Map _map;
    };
}

#include "../tcc/MappedParser.tcc"
