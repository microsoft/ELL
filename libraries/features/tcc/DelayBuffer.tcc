////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DelayBuffer.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DelayBuffer.h"

namespace features
{
    template <typename T, int N>
    void DelayBuffer<T,N>::AddSample(T val)
    {
        buffer.Append(val);
    }

    template <typename T, int N>
    T DelayBuffer<T,N>::GetDelayedSample(int delay)
    {
        return buffer[-delay];
    }
}

