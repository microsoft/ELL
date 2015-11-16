// SparseVectorParsingIterator.h

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

    class SparseVectorParsingIterator
    {
    public:
        SparseVectorParsingIterator(stringstream&& sstream);

        /// Default move ctor
        ///
        SparseVectorParsingIterator(SparseVectorParsingIterator&&) = default;

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
