////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [project]
//  File:     DelayBuffer.h (features)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "FastMath.h"
#include "RingBuffer.h"

// TODO: use ring buffer of Size N+1? With delay of N, we want to get sample delayed by any amount in [0,N] (no?)

namespace features
{
    template <typename T, int N>
    class DelayBuffer
    {
    public:
        void AddSample(T val);
        T GetDelayedSample(int delay);

    private:
        RingBufferArray<T, N> buffer;
    };
}

#include "../tcc/DelayBuffer.tcc"