////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DynamicArray.h (utilities)
//  Authors:  Umesh Madan, Ofer Dekel
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
    ///<summary> A simple wrapper around stl vector </summary>
    template <typename EntryType>
    class DynamicArray
    {
    public:
        DynamicArray() = default;

        /// <summary> Constructs an array from a list of entires. </summary>
        ///
        /// <param name="entries"> A list of entries. </param>
        DynamicArray(const std::initializer_list<EntryType>& entries);

        /// <summary> Appends an entry to the end of the array. </summary>
        ///
        /// <param name="entry"> The entry to append. </param>
        void Append(EntryType entry);

        /// <summary> Appends a list of entries to the end of the array. </summary>
        ///
        /// <param name="entries"> The list of entries to append. </param>
        void Append(std::initializer_list<EntryType> entries);

        /// <summary> Gets the size of the array. </summary>
        ///
        /// <returns> The size. </returns>
        size_t Size() const { return _entries.size(); }

        /// <summary> Clears the array and fills it with new entries. </summary>
        ///
        /// <param name="entries"> The list of entries. </param>
        void Replace(std::initializer_list<EntryType> entries);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the entry. </param>
        ///
        /// <returns> The entry. </returns>
        const EntryType& operator[](size_t index) const;

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the entry. </param>
        ///
        /// <returns> The entry. </returns>
        EntryType& operator[](size_t index);

        /// <summary> Gets an stl iterator that points to the beginning of the underlying stl vector. </summary>
        typename std::vector<EntryType>::const_iterator Begin() const { return _entries.begin(); }

        /// <summary> Gets an stl iterator that points to the end of the underlying stl vector. </summary>
        typename std::vector<EntryType>::const_iterator End() const { return _entries.end(); }

        /// <summary> Erases all the entries in the array and sets its size to zero. </summary>
        void Clear() { _entries.clear(); }

        /// <summary> Returns a reference to the underlying stl vector. </summary>
        const std::vector<EntryType>& GetVector() const { return _entries; }

    private:
        std::vector<EntryType> _entries;
    };
}
}
#include "../tcc/DynamicArray.tcc"