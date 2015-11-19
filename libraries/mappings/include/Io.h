// Io.h

#pragma once

#include "Mapping.h"

#include <iostream>
using std::istream;
using std::ostream;

#include <memory>
using std::shared_ptr;

namespace mappings
{
    /// a utility class that reads and writes mappings to streams
    ///
    class Io
    {
    public:

        /// writes a Mapping to a stream
        ///
        static void Write(ostream& os, shared_ptr<Mapping> map);

        /// reads a Mapping from a stream
        ///
        static shared_ptr<Mapping> Read(istream& is);

        /// reads a column Mapping from a stream
        /// num_layers = 0 keeps the entire Mapping
        /// num_layers = positive keeps only the specified number of layers, starting from the top
        /// num_layers = negative subtracts the specified number of layers, starting from the bottom
        static shared_ptr<Column> ReadColumn(istream& is, int num_layers);
    };
}