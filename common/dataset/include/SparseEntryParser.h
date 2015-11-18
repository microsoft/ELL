// SparseEntryParser.h

#pragma once

#include "IndexValue.h"

#include <string>
using std::string;

#include <iostream>
using std::ostream;

#include <memory>
using std::shared_ptr;

namespace dataset
{
    /// Parses and iterates over a stringstream of index-value pairs
    ///
    class SparseEntryParser
    {
    public:

        class Iterator
        {
        public:
            /// default copy ctor
            ///
            Iterator(Iterator&) = default;

            /// Default move ctor
            ///
            Iterator(Iterator&&) = default;

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
            shared_ptr<const string> _spExampleString; // hold on to the shared ptr to make sure that string is not deleted
            const char* _currentPos = 0;
            IndexValue _currentIndexValue;
            bool _isValid = true;

            // Private constructor that can only be called from SparseEntryParser
            Iterator(shared_ptr<const string> spExampleString, const char* pStr);
            friend class SparseEntryParser;
        };

        Iterator GetIterator(shared_ptr<const string> spExampleString, const char* pStr) const;
    };

}
