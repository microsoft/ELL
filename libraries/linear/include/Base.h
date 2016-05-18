////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Base.h (linear)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <iostream>

/// <summary> linear namespace </summary>
namespace linear
{
    /// <summary> Base class for all other classes in the linear library. </summary>
    class Base
    {
    public:

        virtual ~Base() = default;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const = 0;
    };

    /// <summary> Generic streaming operator that calls the derived class's Print() function. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="v"> The object to print. </param>
    ///
    /// <returns> The stream. </returns>
    std::ostream& operator<<(std::ostream& os, const Base& v);
}
