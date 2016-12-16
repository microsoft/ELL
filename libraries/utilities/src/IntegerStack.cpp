////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IntegerStack.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IntegerStack.h"

// stl
#include <cassert>

namespace ell
{
namespace utilities
{
    size_t IntegerStack::Pop()
    {
        size_t top;

        if (_stack.empty())
        {
            top = _smallestUnpopped;
            _smallestUnpopped++;
        }
        else
        {
            top = _stack.top();
            _stack.pop();
        }

        return top;
    }
    size_t IntegerStack::Top() const
    {
        if (_stack.empty())
        {
            return _smallestUnpopped;
        }
        else
        {
            return _stack.top();
        }
    }
    void IntegerStack::Push(size_t value)
    {
        _stack.push(value);

        assert(value < _smallestUnpopped);
    }
}
}
