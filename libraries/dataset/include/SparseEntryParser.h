// SparseEntryParser.h

#pragma once

// types
#include "types.h"

// stl
#include <string>
#include <iostream>
#include <memory>

namespace dataset
{
    /// Parses and iterates over a stringstream of index-value pairs
    ///
    class SparseEntryParser
    {
    public:

        class Iterator : public IIndexValueIterator
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
            IndexValue Get() const;

        private:
            std::shared_ptr<const std::string> _spExampleString; // hold on to the shared ptr to make sure that std::string is not deleted
            const char* _currentPos = 0;
            IndexValue _currentIndexValue;
            bool _isValid = true;

            // Private constructor that can only be called from SparseEntryParser
            Iterator(std::shared_ptr<const std::string> spExampleString, const char* pStr);
            friend class SparseEntryParser;
        };

        Iterator GetIterator(std::shared_ptr<const std::string> spExampleString, const char* pStr) const;
    };

}
