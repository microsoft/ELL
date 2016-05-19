////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryErrorAggregator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>

namespace evaluators
{
    class BinaryErrorAggregator
    {
    public:

        BinaryErrorAggregator() = default;
        BinaryErrorAggregator(const BinaryErrorAggregator&) = delete;
        BinaryErrorAggregator(BinaryErrorAggregator&&) = default;

        void Update(bool error) 
        { 
            ++_count; 
            if(error)++_numErrors; 
        }

    private:
        uint64_t _count;
        uint64_t _numErrors;
    };
}