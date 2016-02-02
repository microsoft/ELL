// IntegerList.h

#pragma once

// types
#include "types.h"

// stl
#include <cstdint>
#include <vector>

namespace dataset
{
    /// A non-decreasing list of nonegative integers, with a forward Iterator.
    ///
    class IntegerList
    {
    public:

        typedef std::vector<uint64>::const_iterator vector_iterator;

        /// A read-only forward iterator for the IntegerList.
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

            /// \returns True if the iterator is currently pointing to a valid iterate
            ///
            bool IsValid() const;

            /// Proceeds to the Next iterate
            ///
            void Next();

            /// Returns the value of the current iterate
            ///
            uint64 Get() const;

        private:

            /// private ctor, can only be called from IntegerList class
            Iterator(const vector_iterator& begin, const vector_iterator& end);
            friend class IntegerList;

            // members
            vector_iterator _begin;
            vector_iterator _end;
        };

        /// Default Constructor. Constructs an empty list.
        ///
        IntegerList();

        /// Move Constructor
        ///
        IntegerList(IntegerList&& other) = default;

        /// Deleted copy constructor
        ///
        IntegerList(const IntegerList&) = delete;

        /// Default Destructor.
        ///
        ~IntegerList() = default;

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

        /// Deletes all of the vector content and sets its Size to zero
        ///
        void Reset();

        /// \Returns a Iterator that points to the beginning of the list.
        ///
        Iterator GetIterator() const;

        ///@{
        void operator= (const IntegerList&) = delete;
        ///@}

    private:
        std::vector<uint64> _list;
    };
}
