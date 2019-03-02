////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RingBuffer.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstring> // for size_t
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> A simple fixed-length ring buffer.  A ring buffer holds a fixed size
    /// of the most recent items appended and older items are forgotten. </summary>
    template <typename T>
    class RingBuffer
    {
    public:
        /// <summary> The constructor size parameter determines how much history this buffer can remember.
        /// This size is fixed and Append will truncate input so it only ever remembers this many items.
        /// The buffer is initialized with zeros. </summary>
        ///
        /// <param name="size"> The size of the buffer.  </param>
        RingBuffer(size_t size);

        /// <summary> Get the size of the buffer.  This is fixed at construction time and does not change.  </summary>
        ///
        /// <returns> The size of the buffer. </returns>
        size_t Size() const;

        /// <summary> Access an element in the buffer in order of most recent to oldest item.</summary>
        ///
        /// <param name="index"> The index into the buffer. </param>
        ///
        /// <returns> A mutable reference to the element at the given index. </returns>
        T& operator[](size_t index);

        /// <summary> Access an element in the buffer in order of most recent to oldest item.</summary>
        ///
        /// <param name="index"> The index into the buffer. </param>
        ///
        /// <returns> A const reference to the element at the given index. </returns>
        const T& operator[](size_t index) const;

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
        size_t GetBufferIndex(size_t inputIndex) const;

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
    size_t RingBuffer<T>::GetBufferIndex(size_t entryIndex) const
    {
        int size = static_cast<int>(Size());
        return (size + _currentPos - entryIndex) % (int)size; // Note: it's important the RHS argument to % is a signed int
    }

    template <typename T>
    const T& RingBuffer<T>::operator[](size_t index) const
    {
        return _buffer[GetBufferIndex(index)];
    }

    template <typename T>
    T& RingBuffer<T>::operator[](size_t index)
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
