////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForAll.cpp (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/ForAll.h"

namespace ell
{
namespace value::loopnests
{
    ForAll::ForAll(IterationDomain domain) :
        _loops(domain)
    {
    }

    ForAll& ForAll::AddKernel(const Kernel& kernel)
    {
        _loops.AddKernel(kernel, LoopNest::ConstraintType::constraint);
        return *this;
    }

    ForAll& ForAll::AddKernel(const Kernel& kernel, const CodePositionConstraints& where)
    {
        _loops.AddKernel(kernel, where);
        return *this;
    }

    ForAll& ForAll::Split(const Index& dimension, int size)
    {
        _loops.Split(dimension, size);
        return *this;
    }

    ForAll& ForAll::SetLoopOrder(const std::vector<Index>& order)
    {
        _loops.SetLoopOrder(order);
        return *this;
    }

    const LoopNest& ForAll::GetNest() const
    {
        return _loops;
    }
} // namespace value::loopnests
} // namespace ell