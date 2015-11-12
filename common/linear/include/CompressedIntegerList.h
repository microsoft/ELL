// CompressedIntegerList.h

#pragma once

#include "types.h"

#include <vector>
using std::vector;

#include <iterator>
using std::iterator;
using std::forward_iterator_tag;

namespace linear
{
    typedef uint64_t uint;
    typedef uint8_t uint8;

    /// A non-decreasing list of nonegative integers, with a forward ConstIterator, stored in a compressed delta enconding.
    ///
    class CompressedIntegerList
    {
    public:

        /// A read-only forward iterator for the CompressedIntegerList.
        ///
        class ConstIterator
        {
        public:

            /// Default copy ctor
            ///
            ConstIterator(const ConstIterator&) = default;

            /// Default move ctor
            ///
            ConstIterator(ConstIterator&&) = default;

            /// \returns True if the iterator is currently pointing to a valid iterate
            ///
            bool IsValid() const;

            /// Proceeds to the Next iterate
            ///
            void Next();

            /// Returns the value of the current iterate
            ///
            uint GetValue() const;

        private:

            /// private ctor, can only be called from CompressedIntegerList class
            ConstIterator(const uint8 *iter, const uint8 *end);
            friend class CompressedIntegerList;

            // members
            const uint8* _iter;
            const uint8* _end;
            uint _value;
            uint _iter_increment;
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
        uint Size() const;

        /// Allocates a specified number of entires to the list
        ///
        void Reserve(uint size);

        /// \returns The maximal integer in the list
        ///
        uint Max() const;

        /// Appends an integer to the end of the list.
        ///
        void PushBack(uint value);

        /// Deletes all of the vector content and sets its Size to zero
        ///
        void Reset();

        /// \Returns a ConstIterator that points to the beginning of the list.
        ///
        ConstIterator GetConstIterator() const;

        ///@{
        void operator= (const CompressedIntegerList&) = delete;
        ///@}

    private:
        vector<uint8> _mem;
        uint _last;
        uint _size;
    };
}
