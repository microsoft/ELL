////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IntegerModel.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <stack>

namespace ell
{
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
        size_t Pop();

        /// <summary> Gets the top element in the stack. </summary>
        ///
        /// <returns> An element at the top of the stack. </returns>
        size_t Top() const;

        /// <summary> Pushes an object onto this stack. </summary>
        ///
        /// <param name="value"> The value to push. This must be a value that was previously popped. </param>
        void Push(size_t value);

        /// <summary> Query if the top value in the stack is novel, which means that it has never been popped before. </summary>
        ///
        /// <returns> true if the top value is novel, false otherwise. </returns>
        bool IsTopNovel() const { return _stack.empty(); }

    private:
        size_t _smallestUnpopped = 0;
        std::stack<size_t> _stack;
    };
}
}
