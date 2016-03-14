////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     MappedParser.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
    /// <summary> Wraps any other parser and passes it through a map. </summary>
    ///
    /// <typeparam name="ternalParserType"> Type of the internal parser type. </typeparam>
    template<typename InternalParserType>
    class MappedParser
    {
    public:
        /// <summary> Copy constructor. </summary>
        MappedParser(const MappedParser&) = default;

        /// <summary> Move constructor. </summary>
        MappedParser(MappedParser&&) = default;

        /// <summary> Constructs an instance of MappedParser. </summary>
        ///
        /// <param name="internalParser"> The internal parser. </param>
        /// <param name="map"> The map. </param>
        /// <param name="outputCoordinates"> The output coordinates. </param>
        MappedParser(const InternalParserType& internalParser, const std::shared_ptr<layers::Map>& map, layers::CoordinateList outputCoordinates);

        /// <summary> Gets an iterator that parses the given std::string. </summary>
        ///
        /// <param name="spExampleString"> The sp example string. </param>
        /// <param name="pStr"> The string. </param>
        ///
        /// <returns> The iterator. </returns>
        layers::Map::OutputIterator GetIterator(std::shared_ptr<const std::string> spExampleString, const char* pStr) const;

    private:
        InternalParserType _internalParser;
        std::shared_ptr<layers::Map> _map; 
        layers::CoordinateList _outputCoordinates;
    };
}

#include "../tcc/MappedParser.tcc"
