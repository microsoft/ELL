// Io.h

#pragma once

#include "Layer.h"

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
        static void Write(ostream& os, shared_ptr<Layer> map);

        /// reads a Mapping from a stream
        ///
        static shared_ptr<Layer> Read(istream& is);

        ///// reads a column Mapping from a stream
        ///// numLayers = 0 keeps the entire Mapping
        ///// numLayers = positive keeps only the specified number of layers, starting from the top
        ///// numLayers = negative subtracts the specified number of layers, starting from the bottom
        //static shared_ptr<Column> ReadColumn(istream& is, int numLayers);
    };
}