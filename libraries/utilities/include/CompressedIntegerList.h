////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CompressedIntegerList.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <iterator>
#include <vector>

namespace emll
{
namespace utilities
{
    /// <summary> A non-decreasing list of nonegative integers, with a forward Iterator, stored in a
    /// compressed delta enconding. </summary>
    class CompressedIntegerList
    {
    public:
        /// <summary> A read-only forward std::iterator for the CompressedIntegerList. </summary>
        class Iterator
        {
        public:
            Iterator() = default;

            Iterator(const Iterator&) = default;

            Iterator(Iterator&&) = default;

            // Returns true if the std::iterator is currently pointing to a valid iterate

            /// <summary> Query if this object input stream valid. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const { return _iter < _end; }

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> Returns the value of the current iterate. </summary>
            ///
            /// <returns> An uint64_t. </returns>
            uint64_t Get() const { return _value; }

        private:
            // private ctor, can only be called from CompressedIntegerList class
            Iterator(const uint8_t* iter, const uint8_t* end);
            friend class CompressedIntegerList;

            // members
            const uint8_t* _iter;
            const uint8_t* _end;
            uint64_t _value;
            uint64_t _iter_increment;
        };

        /// <summary> Default Constructor. Constructs an empty list. </summary>
        CompressedIntegerList();

        CompressedIntegerList(CompressedIntegerList&& other) = default;

        CompressedIntegerList(const CompressedIntegerList&) = default;

        ~CompressedIntegerList() = default;

        void operator=(const CompressedIntegerList&) = delete;

        /// <summary> Returns The number of entries in the list. </summary>
        ///
        /// <returns> An uint64_t. </returns>
        uint64_t Size() const;

        /// <summary> Allocates a specified number of entires to the list. </summary>
        ///
        /// <param name="size"> The size. </param>
        void Reserve(uint64_t size);

        /// <summary> Returns The maximal integer in the list. </summary>
        ///
        /// <returns> The maximum value. </returns>
        uint64_t Max() const;

        /// <summary> Appends an integer to the end of the list. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Append(uint64_t value);

        /// <summary> Deletes all of the std::vector content and sets its Size to zero. </summary>
        void Reset();

        /// <summary> Returns an `Iterator` that points to the beginning of the list. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const { return Iterator(_data.data(), _data.data() + _data.size()); }

    private:
        std::vector<uint8_t> _data;
        uint64_t _last;
        uint64_t _size;
    };
}
}
