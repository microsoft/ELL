////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IntegerList.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <vector>

namespace utilities
{
    /// <summary> A non-decreasing list of nonegative integers, with a forward Iterator. </summary>
    class IntegerList
    {
    public:

        /// <summary> Defines an alias representing the vector iterator. </summary>
        typedef std::vector<uint64_t>::const_iterator vector_iterator;

        /// <summary> A read-only forward iterator for the IntegerList. </summary>
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

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const;

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> Returns the value of the current iterate. </summary>
            ///
            /// <returns> An uint64_t. </returns>
            uint64_t Get() const;

        private:

            // private ctor, can only be called from IntegerList class. 
            Iterator(const vector_iterator& begin, const vector_iterator& end);
            friend class IntegerList;

            // members
            vector_iterator _begin;
            vector_iterator _end;
        };

        /// <summary> Default Constructor. Constructs an empty list. </summary>
        IntegerList();

        /// <summary> Move Constructor. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        IntegerList(IntegerList&& other) = default;

        /// <summary> Deleted copy constructor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        IntegerList(const IntegerList&) = default;

        /// <summary> Default Destructor. </summary>
        ~IntegerList() = default;

        /// <summary> Deleted assignment operator. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        void operator= (const IntegerList&) = delete;

        /// <summary> Gets the number of entries in the list. </summary>
        ///
        /// <returns> An uint64_t. </returns>
        uint64_t Size() const;

        /// <summary> Allocates a specified number of entires to the list. </summary>
        ///
        /// <param name="size"> The size. </param>
        void Reserve(uint64_t size);

        /// <summary> Gets the maximal integer in the list. </summary>
        ///
        /// <returns> The maximum value. </returns>
        uint64_t Max() const;

        /// <summary> Appends an integer to the end of the list. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Append(uint64_t value);

        /// <summary> Deletes all of the vector content and sets its Size to zero. </summary>
        void Reset();

        /// <summary> Gets Iterator that points to the beginning of the list. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const;


    private:
        // The list
        std::vector<uint64_t> _list;
    };
}
