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
    /// <summary> A non-decreasing list of nonegative integers, with a forward Iterator, stored in a
    /// compressed delta enconding. </summary>
    class CompressedIntegerList // TODO move to utilities
    {
    public:

        /// <summary> A read-only forward std::iterator for the CompressedIntegerList. </summary>
        class Iterator
        {
        public:

            /// <summary> Default copy ctor. </summary>
            ///
            /// <param name="parameter1"> The first parameter. </param>
            Iterator(const Iterator&) = default;

            /// <summary> Default move ctor. </summary>
            ///
            /// <param name="parameter1"> [in,out] The first parameter. </param>
            Iterator(Iterator&&) = default;

            // \returns True if the std::iterator is currently pointing to a valid iterate

            /// <summary> Query if this object input stream valid. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const;

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> Returns the value of the current iterate. </summary>
            ///
            /// <returns> An uint64. </returns>
            uint64 Get() const;

        private:

            // private ctor, can only be called from CompressedIntegerList class
            Iterator(const uint8 *iter, const uint8 *end);
            friend class CompressedIntegerList;

            // members
            const uint8* _iter;
            const uint8* _end;
            uint64 _value;
            uint64 _iter_increment;
        };

        /// <summary> Default Constructor. Constructs an empty list. </summary>
        CompressedIntegerList();

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        CompressedIntegerList(CompressedIntegerList&& other) = default;

        /// <summary> Deleted copy constructor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        CompressedIntegerList(const CompressedIntegerList&) = delete;

        /// <summary> Default Destructor. </summary>
        ~CompressedIntegerList() = default;

        /// <summary> Deleted assignment operator. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        void operator= (const CompressedIntegerList&) = delete;

        /// <summary> \returns The number of entries in the list. </summary>
        ///
        /// <returns> An uint64. </returns>
        uint64 Size() const;

        /// <summary> Allocates a specified number of entires to the list. </summary>
        ///
        /// <param name="size"> The size. </param>
        void Reserve(uint64 size);

        /// <summary> \returns The maximal integer in the list. </summary>
        ///
        /// <returns> The maximum value. </returns>
        uint64 Max() const;

        /// <summary> Appends an integer to the end of the list. </summary>
        ///
        /// <param name="value"> The value. </param>
        void PushBack(uint64 value);

        /// <summary> Deletes all of the std::vector content and sets its Size to zero. </summary>
        void Reset();

        /// <summary> \Returns a Iterator that points to the beginning of the list. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const;

    private:
        std::vector<uint8> _data;
        uint64 _last;
        uint64 _size;
    };
}
