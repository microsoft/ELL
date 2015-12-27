// MappedParser.h

#pragma once

#include "types.h"

#include <string>
using std::string;

#include <memory>
using std::shared_ptr;

namespace dataset
{
    /// Wraps any other parser and passes it through a map
    ///
    template<typename InternalParserType>
    class MappedParser
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
            indexValue Get() const;

        private:

            // Private constructor that can only be called from SparseEntryParser
            Iterator();
            friend class MappedParser;

        };

        Iterator GetIterator(shared_ptr<const string> spExampleString, const char* pStr) const;

    private:
        InternalParserType _internalParser;
    };
}

#include "../tcc/MappedParser.tcc"
