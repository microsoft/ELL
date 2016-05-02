////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     IntegerModel.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <stack>

namespace utilities
{
    /// <summary> Implements a stack of integers, which is initialized to be 0,1,2,.... Namely, the
    /// first Pop() returns 0, the second returns 1, etc. If a value is pushed onto the stack, it
    /// occupies the top of the stack and follows the standard stack semantics. </summary>
    class IntegerStack
    {
    public:
        /// <summary> Removes and returns the top-of-stack object. </summary>
        ///
        /// <returns> The previous top-of-stack object. </returns>
        uint64_t Pop();

        /// <summary> Gets the top element in the stack. </summary>
        ///
        /// <returns> An element at the top of the stack. </returns>
        uint64_t Top() const;

        /// <summary> Pushes an object onto this stack. </summary>
        ///
        /// <param name="value"> The value to push. This must be a value that was previously popped. </param>
        void Push(uint64_t value);

        /// <summary> Query if the top value in the stack is novel, which means that it has never been popped before. </summary>
        ///
        /// <returns> true if the top value is novel, false otherwise. </returns>
        bool IsTopNovel() const;

    private:
        uint64_t _smallestUnpopped = 0;
        std::stack<uint64_t> _stack;
    };
}