////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForAll.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LoopNest.h"

namespace ell
{
namespace value::loopnests
{
    //
    // Syntactic sugar class for stringing together loop nest calls
    //
    class ForAll
    {
    public:
        ForAll(const ForAll& other) = delete;
        ForAll(ForAll&& other) = default;
        ForAll(IterationDomain domain);
        ForAll& operator=(const ForAll& other) = delete;
        ForAll& operator=(ForAll&& other) = default;

        ForAll& AddKernel(const Kernel& kernel);
        ForAll& AddKernel(const Kernel& kernel, const CodePositionConstraints& where);
        ForAll& Split(const Index& dimension, int size);
        ForAll& SetLoopOrder(const std::vector<Index>& order);

        const LoopNest& GetNest() const;

    private:
        LoopNest _loops;
    };
} // namespace value::loopnests
} // namespace ell