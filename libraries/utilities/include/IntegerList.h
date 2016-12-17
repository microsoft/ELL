////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IntegerList.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> A non-decreasing list of nonegative integers, with a forward Iterator. </summary>
    class IntegerList
    {
    public:
        /// <summary> Defines an alias representing the vector iterator. </summary>
        typedef std::vector<size_t>::const_iterator vector_iterator;

        /// <summary> A read-only forward iterator for the IntegerList. </summary>
        class Iterator
        {
        public:
            Iterator(const Iterator&) = default;

            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const { return _begin < _end; }

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next() { ++_begin; }

            /// <summary> Returns the value of the current iterate. </summary>
            ///
            /// <returns> An size_t. </returns>
            size_t Get() const { return *_begin; }

        private:
            // private ctor, can only be called from IntegerList class.
            Iterator(const vector_iterator& begin, const vector_iterator& end);
            friend class IntegerList;

            // members
            vector_iterator _begin;
            vector_iterator _end;
        };

        IntegerList() = default;

        IntegerList(IntegerList&& other) = default;

        IntegerList(const IntegerList&) = default;

        ~IntegerList() = default;

        void operator=(const IntegerList&) = delete;

        /// <summary> Gets the number of entries in the list. </summary>
        ///
        /// <returns> An size_t. </returns>
        size_t Size() const { return (size_t)_list.size(); }

        /// <summary> Allocates a specified number of entires to the list. </summary>
        ///
        /// <param name="size"> The size. </param>
        void Reserve(size_t size);

        /// <summary> Gets the maximal integer in the list. </summary>
        ///
        /// <returns> The maximum value. </returns>
        size_t Max() const;

        /// <summary> Appends an integer to the end of the list. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Append(size_t value);

        /// <summary> Deletes all of the vector content and sets its Size to zero. </summary>
        void Reset() { _list.resize(0); }

        /// <summary> Gets Iterator that points to the beginning of the list. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const;

    private:
        // The list
        std::vector<size_t> _list;
    };
}
}
