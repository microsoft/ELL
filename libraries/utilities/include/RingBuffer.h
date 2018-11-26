////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RingBuffer.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstring> // for size_t
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> A simple fixed-length ring buffer. </summary>
    template <typename T>
    class RingBuffer
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="size"> The length of the buffer. </param>
        RingBuffer(size_t size);

        /// <summary> Get the length of the buffer </summary>
        ///
        /// <returns> The length of the buffer. </returns>
        size_t Size() const;

        /// <summary> Access an element in the buffer </summary>
        ///
        /// <param name="index"> The index into the buffer. </param>
        ///
        /// <returns> A mutable reference to the element at the given index. </returns>
        T& operator[](int index);

        /// <summary> Access an element in the buffer </summary>
        ///
        /// <param name="index"> The index into the buffer. </param>
        ///
        /// <returns> A const reference to the element at the given index. </returns>
        const T& operator[](int index) const;

        /// <summary> Add a new element at the end of the buffer. </summary>
        ///
        /// <param name="value"> The value to add to the end. </param>
        void Append(const T& value);

        /// <summary> Fill the buffer with a value. </summary>
        ///
        /// <param name="value"> The value to fill the buffer with. </param>
        void Fill(const T& value);

        /// <summary> Resize the ring buffer and reset the current position to zero. </summary>
        void Resize(size_t size);

    private:
        size_t GetBufferIndex(int inputIndex) const;

        std::vector<T> _buffer;
        size_t _currentPos = 0;
    };
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    //
    // RingBuffer class
    //
    template <typename T>
    RingBuffer<T>::RingBuffer(size_t size) :
        _buffer(size),
        _currentPos(0)
    {
    }

    template <typename T>
    size_t RingBuffer<T>::Size() const
    {
        return _buffer.size();
    }

    template <typename T>
    void RingBuffer<T>::Resize(size_t size)
    {
        _buffer.resize(size);
        _currentPos = 0;
    }

    template <typename T>
    size_t RingBuffer<T>::GetBufferIndex(int entryIndex) const
    {
        int size = static_cast<int>(Size());
        return (size + _currentPos - entryIndex) % (int)size; // Note: it's important the RHS argument to % is a signed int
    }

    template <typename T>
    const T& RingBuffer<T>::operator[](int index) const
    {
        return _buffer[GetBufferIndex(index)];
    }

    template <typename T>
    T& RingBuffer<T>::operator[](int index)
    {
        return _buffer[GetBufferIndex(index)];
    }

    template <typename T>
    void RingBuffer<T>::Append(const T& val)
    {
        int size = static_cast<int>(Size());
        _currentPos = (_currentPos + 1) % size;
        _buffer[_currentPos] = val;
    }

    template <typename T>
    void RingBuffer<T>::Fill(const T& val)
    {
        std::fill(_buffer.begin(), _buffer.end(), val);
    }
} // namespace utilities
} // namespace ell

#pragma endregion implementation
