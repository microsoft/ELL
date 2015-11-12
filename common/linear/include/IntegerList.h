// IntegerList.h

#pragma once

#include <cstdint>
#include <vector>

namespace linear
{
    typedef uint64_t uint;

    /// A non-decreasing list of nonegative integers, with a forward ConstIterator.
    ///
    class IntegerList
    {
    public:

        typedef std::vector<uint>::const_iterator vector_iterator;

        /// A read-only forward iterator for the IntegerList.
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

            /// private ctor, can only be called from IntegerList class
            ConstIterator(const vector_iterator& begin, const vector_iterator& end);
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
        void operator= (const IntegerList&) = delete;
        ///@}

    private:
        std::vector<uint> _list;
    };
}
