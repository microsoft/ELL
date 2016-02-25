////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StringFormat.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// stl
#include <iostream>

namespace utilities
{
    /// <summary> Formats a string. </summary>
    ///
    /// <typeparam name="ArgType"> Type of the argument type. </typeparam>
    /// <typeparam name="ArgTypes"> Type of the argument types. </typeparam>
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="cstr"> The cstr. </param>
    /// <param name="arg"> The argument. </param>
    /// <param name="...args"> The .. .args. </param>
    template<typename ArgType, typename ... ArgTypes>
    void StringFormat(std::ostream& os, const char* cstr, ArgType arg, ArgTypes ...args);

    /// <summary> Formats a string (end condition in the template recurrsion). </summary>
    ///
    /// <typeparam name="ArgTypes"> Type of the argument types. </typeparam>
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="cstr"> The cstr. </param>
    /// <param name="...args"> The .. .args. </param>
    template<typename ... ArgTypes>
    void StringFormat(std::ostream& os, const char* cstr, ArgTypes ...args);

    template<typename ... ArgTypes>

    /// <summary> Formats a string. </summary>
    ///
    /// <param name="cstr"> The cstr. </param>
    /// <param name="...args"> The .. .args. </param>
    ///
    /// <returns> A std::string. </returns>
    std::string StringFormat(const char* cstr, ArgTypes ...args);
}

#include "../tcc/StringFormat.tcc"
