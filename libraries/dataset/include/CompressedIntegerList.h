////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompressedIntegerList.h (dataset)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// stl
#include <vector>
#include <iterator>

namespace dataset
{
    /// A non-decreasing list of nonegative integers, with a forward Iterator, stored in a compressed delta enconding.
    ///
    class CompressedIntegerList // TODO move to utilities
    {
    public:

        /// A read-only forward std::iterator for the CompressedIntegerList.
        ///
        class Iterator
        {
        public:

            /// Default copy ctor
            ///
            Iterator(const Iterator&) = default;

            /// Default move ctor
            ///
            Iterator(Iterator&&) = default;

            /// \returns True if the std::iterator is currently pointing to a valid iterate
            ///
            bool IsValid() const;

            /// Proceeds to the Next iterate
            ///
            void Next();

            /// Returns the value of the current iterate
            ///
            uint64 Get() const;

        private:

            /// private ctor, can only be called from CompressedIntegerList class
            Iterator(const uint8 *iter, const uint8 *end);
            friend class CompressedIntegerList;

            // members
            const uint8* _iter;
            const uint8* _end;
            uint64 _value;
            uint64 _iter_increment;
        };

        /// Default Constructor. Constructs an empty list.
        ///
        CompressedIntegerList();

        /// Move constructor
        ///
        CompressedIntegerList(CompressedIntegerList&& other) = default;

        /// Deleted copy constructor
        ///
        CompressedIntegerList(const CompressedIntegerList&) = delete;

        /// Default Destructor.
        ///
        ~CompressedIntegerList() = default;

        /// \returns The number of entries in the list
        ///
        uint64 Size() const;

        /// Allocates a specified number of entires to the list
        ///
        void Reserve(uint64 size);

        /// \returns The maximal integer in the list
        ///
        uint64 Max() const;

        /// Appends an integer to the end of the list.
        ///
        void PushBack(uint64 value);

        /// Deletes all of the std::vector content and sets its Size to zero
        ///
        void Reset();

        /// \Returns a Iterator that points to the beginning of the list.
        ///
        Iterator GetIterator() const;

        ///@{
        void operator= (const CompressedIntegerList&) = delete;
        ///@}

    private:
        std::vector<uint8> _data;
        uint64 _last;
        uint64 _size;
    };
}
