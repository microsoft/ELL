////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Base.h (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>

namespace linear
{
    /// Base class for everything in this library
    ///
    class Base
    {
    public:

        /// default virtual destructor
        ///
        virtual ~Base() = default;

        /// Human readable printout to an output stream
        ///
        virtual void Print(std::ostream& os) const = 0;
    };

    /// Generic operator<< that calls each class's Print() function
    ///
    std::ostream& operator<<(std::ostream& os, const Base& v);
}
