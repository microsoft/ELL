////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [project]
//  File:     RingBuffer.h (features)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <vector>

namespace features
{
    // #### TODO: unify these two RingBuffer classes

    // Ring buffer with size only known at runtime
    template <typename T>
    class RingBuffer
    {
    public:
        RingBuffer(size_t size);
        size_t Size() const;
        T operator[](int index) const;
        void Append(const T& val);

    private:
        std::vector<T> _buffer;
        size_t _currentPos;
        size_t _size; 
    };

    // Ring buffer with size known at compile-time
    template <typename T, int N>
    class RingBufferArray
    {
    public:
        RingBufferArray();
        size_t Size() const;
        T operator[](int index) const;
        void Append(const T& val);

    private:    
        std::array<T, N> _arr;
        int _currentPos = 0;
    };
}

#include "../tcc/RingBuffer.tcc"