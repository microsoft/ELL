////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     SparseEntryParser.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// linear
#include "IndexValue.h"

// stl
#include <string>
#include <iostream>
#include <memory>

namespace dataset
{
    /// <summary> Parses and iterates over a stringstream of index-value pairs. </summary>
    class SparseEntryParser
    {
    public:

        class Iterator : public linear::IIndexValueIterator
        {
        public:

            /// <summary> default copy ctor. </summary>
            ///
            /// <param name="parameter1"> [in,out] The first parameter. </param>
            Iterator(Iterator&) = default;

            /// <summary> Default move ctor. </summary>
            ///
            /// <param name="parameter1"> [in,out] The first parameter. </param>
            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const;

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> \returns The current index-value pair. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            linear::IndexValue Get() const;

        private:
            std::shared_ptr<const std::string> _spExampleString; // hold on to the shared ptr to make sure that std::string is not deleted
            const char* _currentPos = 0;
            linear::IndexValue _currentIndexValue;
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
