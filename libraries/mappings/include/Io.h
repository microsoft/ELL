// Io.h

#pragma once

#include "Map.h"

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
        static void Write(ostream& os, shared_ptr<Map> map);

        /// reads a Mapping from a stream
        ///
        static shared_ptr<Map> Read(istream& is);
    };
}