////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultidimArray.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/MemoryLayout.h>

#include <initializer_list>
#include <iterator>
#include <vector>

namespace ell
{
/// <summary> A class that represents a multidimensional array stored in a contiguous block of memory
template <typename ElementType>
class MultidimArray
{
public:
    MultidimArray() = default;
    MultidimArray(const ell::utilities::MemoryLayout& layout);
    MultidimArray(std::vector<ElementType> data, const ell::utilities::MemoryLayout& layout);
    MultidimArray(const MultidimArray& other) = default;
    MultidimArray(MultidimArray&& other) = default;
    MultidimArray& operator=(const MultidimArray& other) = default;
    MultidimArray& operator=(MultidimArray&& other) = default;

    ElementType operator[](const ell::utilities::MemoryCoordinates& indices) const;
    ElementType& operator[](const ell::utilities::MemoryCoordinates& indices);
    ElementType operator[](const std::initializer_list<int>& indices) const;
    ElementType& operator[](const std::initializer_list<int>& indices);

    const ell::utilities::MemoryLayout& GetLayout() const { return _layout; }
    const std::vector<ElementType>& GetData() const { return _data; }
    std::vector<ElementType>& GetData() { return _data; }

    class Iterator
    {
    public:
        ell::utilities::MemoryCoordinates GetCoordinates() const;

        using iterator_category = std::random_access_iterator_tag;
        using value_type = ElementType;
        using difference_type = std::ptrdiff_t;
        using size_type = size_t;
        using reference = ElementType&;
        using const_reference = const ElementType&;
        using pointer = ElementType*;
        using const_pointer = const pointer;

        //
        // Required operations for random-access iterators:
        //
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        reference operator*();
        const_reference operator*() const;
        pointer operator->();
        const_pointer operator->() const;
        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);
        Iterator& operator+=(difference_type increment);
        Iterator& operator-=(difference_type increment);
        bool operator<(const Iterator& other) const;
        bool operator>(const Iterator& other) const;
        bool operator>=(const Iterator& other) const;
        bool operator<=(const Iterator& other) const;
        // reference operator[](size_type index);
        // const_reference operator[](size_type index) const;

    private:
        MultidimArray<ElementType>& _array;
        size_t _currIndex;
    };

    Iterator begin() { return { *this, 0 }; }
    Iterator end() { return { *this, _layout.GetMemorySize() }; }

private:
    friend class Iterator;

    std::vector<ElementType> _data;
    ell::utilities::MemoryLayout _layout;
};

#pragma region implementation

template <typename ElementType>
MultidimArray<ElementType>::MultidimArray(const ell::utilities::MemoryLayout& layout) :
    _data(layout.GetMemorySize()),
    _layout(layout)
{}

template <typename ElementType>
MultidimArray<ElementType>::MultidimArray(std::vector<ElementType> data, const ell::utilities::MemoryLayout& layout) :
    _data(data),
    _layout(layout)
{}

template <typename ElementType>
ElementType MultidimArray<ElementType>::operator[](const std::initializer_list<int>& indices) const
{
    return _data[_layout.GetLogicalEntryOffset(indices)];
}

template <typename ElementType>
ElementType& MultidimArray<ElementType>::operator[](const std::initializer_list<int>& indices)
{
    return _data[_layout.GetLogicalEntryOffset(indices)];
}

template <typename ElementType>
ElementType MultidimArray<ElementType>::operator[](const ell::utilities::MemoryCoordinates& indices) const
{
    return _data[_layout.GetLogicalEntryOffset(indices)];
}

template <typename ElementType>
ElementType& MultidimArray<ElementType>::operator[](const ell::utilities::MemoryCoordinates& indices)
{
    return _data[_layout.GetLogicalEntryOffset(indices)];
}

//
// Required operations for random-access iterators:
//

template <typename ElementType>
bool MultidimArray<ElementType>::Iterator::operator==(const MultidimArray<ElementType>::Iterator& other) const
{
    return _currIndex == other._currIndex;
}

template <typename ElementType>
bool MultidimArray<ElementType>::Iterator::operator!=(const MultidimArray<ElementType>::Iterator& other) const
{
    return _currIndex != other._currIndex;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator::reference MultidimArray<ElementType>::Iterator::operator*()
{
    return _array[_currIndex];
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator::const_reference MultidimArray<ElementType>::Iterator::operator*() const
{
    return _array[_currIndex];
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator::pointer MultidimArray<ElementType>::Iterator::operator->()
{
    return _currIndex;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator::const_pointer MultidimArray<ElementType>::Iterator::operator->() const
{
    return _currIndex;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator& MultidimArray<ElementType>::Iterator::operator++()
{
    ++_currIndex;
    return *this;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator MultidimArray<ElementType>::Iterator::operator++(int)
{
    auto temp = _currIndex;
    ++_currIndex;
    return { _array, temp };
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator& MultidimArray<ElementType>::Iterator::operator--()
{
    --_currIndex;
    return *this;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator MultidimArray<ElementType>::Iterator::operator--(int)
{
    auto temp = _currIndex;
    --_currIndex;
    return { _array, temp };
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator& MultidimArray<ElementType>::Iterator::operator+=(difference_type increment)
{
    _currIndex += increment;
    return *this;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator& MultidimArray<ElementType>::Iterator::operator-=(difference_type increment)
{
    _currIndex -= increment;
    return *this;
}

template <typename ElementType>
bool MultidimArray<ElementType>::Iterator::operator>(const MultidimArray<ElementType>::Iterator& other) const
{
    return _currIndex > other._currIndex;
}

template <typename ElementType>
bool MultidimArray<ElementType>::Iterator::operator<(const MultidimArray<ElementType>::Iterator& other) const
{
    return _currIndex < other._currIndex;
}

template <typename ElementType>
bool MultidimArray<ElementType>::Iterator::operator>=(const MultidimArray<ElementType>::Iterator& other) const
{
    return _currIndex >= other._currIndex;
}

template <typename ElementType>
bool MultidimArray<ElementType>::Iterator::operator<=(const MultidimArray<ElementType>::Iterator& other) const
{
    return _currIndex <= other._currIndex;
}

// template <typename ElementType>
// typename MultidimArray<ElementType>::Iterator::reference MultidimArray<ElementType>::Iterator::operator[](typename MultidimArray<ElementType>::Iterator::size_type index)
// {
//     return _currIndex[index];
// }

// template <typename ElementType>
// typename MultidimArray<ElementType>::Iterator::const_reference MultidimArray<ElementType>::Iterator::operator[](typename MultidimArray<ElementType>::Iterator::size_type index) const
// {
//     return _currIndex[index];
// }

//
// Basic math operators defined as standalone functions
//
template <typename ElementType>
typename MultidimArray<ElementType>::Iterator operator+(const typename MultidimArray<ElementType>::Iterator& iterator, typename MultidimArray<ElementType>::Iterator::difference_type increment)
{
    auto result = iterator;
    result += increment;
    return result;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator operator+(typename MultidimArray<ElementType>::Iterator::difference_type increment, const typename MultidimArray<ElementType>::Iterator& iterator)
{
    auto result = iterator;
    result += increment;
    return result;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator operator-(const typename MultidimArray<ElementType>::Iterator& iterator, typename MultidimArray<ElementType>::Iterator::difference_type increment)
{
    auto result = iterator;
    result -= increment;
    return result;
}

template <typename ElementType>
typename MultidimArray<ElementType>::Iterator::difference_type operator-(const typename MultidimArray<ElementType>::Iterator& iterator1, const typename MultidimArray<ElementType>::Iterator& iterator2)
{
    auto p1 = iterator1.GetBaseIterator();
    auto p2 = iterator2.GetBaseIterator();
    return (p1 - p2) / iterator1.GetStride();
}

#pragma endregion implementation
} // namespace ell
