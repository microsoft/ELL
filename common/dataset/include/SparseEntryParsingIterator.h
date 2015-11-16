// SparseEntryParsingIterator.h

#pragma once

#include "IndexValue.h"

#include <string>
using std::string;

#include <iostream>
using std::ostream;

#include <sstream>
using std::stringstream;

namespace dataset
{
    /// Parses and iterates over a stringstream of index-value pairs
    ///
    class SparseEntryParsingIterator
    {
    public:

        /// Constructs an interator by wrapping a stringstream
        ///
        SparseEntryParsingIterator(stringstream&& sstream);

        /// Default move ctor
        ///
        SparseEntryParsingIterator(SparseEntryParsingIterator&&) = default;

        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        bool IsValid() const;

        /// Proceeds to the Next iterate
        ///
        void Next();

        /// \returns The current index-value pair
        ///
        IndexValue GetValue() const;

    private:
        stringstream _sstream;
        IndexValue _currentIndexValue;
    };

}
