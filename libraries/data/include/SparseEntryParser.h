////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SparseEntryParser.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexValue.h"

// stl
#include <iostream>
#include <memory>
#include <string>

namespace ell
{
namespace data
{
    /// <summary> Parses and iterates over a stringstream of index-value pairs. </summary>
    class SparseEntryParser
    {
    public:
        class Iterator : public IIndexValueIterator
        {
        public:
            Iterator(Iterator&) = default;

            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const { return _isValid; }

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> Returns The current index-value pair. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            IndexValue Get() const { return _currentIndexValue; }

        private:
            std::shared_ptr<const std::string> _spExampleString; // hold on to the shared ptr to make sure that std::string is not deleted
            const char* _currentPos = 0;
            IndexValue _currentIndexValue;
            bool _isValid = true;

            // Private constructor that can only be called from SparseEntryParser
            Iterator(std::shared_ptr<const std::string> spExampleString, const char* pStr);
            friend class SparseEntryParser;
        };

        /// <summary> Gets an iterator. </summary>
        ///
        /// <param name="spExampleString"> The sp example string. </param>
        /// <param name="pStr"> The string. </param>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator(std::shared_ptr<const std::string> spExampleString, const char* pStr) const;
    };
}
}
