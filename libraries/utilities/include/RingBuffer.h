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
        
    private:
        size_t GetBufferIndex(int inputIndex) const;

        std::vector<T> _buffer;
        size_t _currentPos = 0;
    };
}
}

#include "../tcc/RingBuffer.tcc"
