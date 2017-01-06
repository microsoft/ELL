////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DynamicArray.tcc (utilities)
//  Authors:  Umesh Madan, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>

namespace ell
{
namespace utilities
{
    template <typename EntryType>
    DynamicArray<EntryType>::DynamicArray(const std::initializer_list<EntryType>& entries)
        : _entries(entries)
    {
    }

    template <typename EntryType>
    void DynamicArray<EntryType>::Append(std::initializer_list<EntryType> entries)
    {
        _entries.insert(_entries.end(), std::move(entries));
    }

    template <typename EntryType>
    void DynamicArray<EntryType>::Append(EntryType entry)
    {
        _entries.push_back(std::move(entry));
    }

    template <typename EntryType>
    void DynamicArray<EntryType>::Replace(std::initializer_list<EntryType> entries)
    {
        _entries.clear();
        Append(std::move(entries));
    }

    template <typename EntryType>
    const EntryType& DynamicArray<EntryType>::operator[](size_t index) const
    {
        return _entries[index];
    }

    template <typename EntryType>
    EntryType& DynamicArray<EntryType>::operator[](size_t index)
    {
        return _entries[index];
    }
}
}