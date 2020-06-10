////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Print.h (value)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Value.h"

#include <string>
#include <vector>

namespace ell
{
namespace value
{
    /// <summary> Emits a print call. </summary>
    ///
    /// <param name="text"> The text to print. </param>
    void Print(const std::string& text);

    /// <summary> Emits a printf call. </summary>
    ///
    /// <param name="arguments"> Arguments to the printf call. </param>
    void Printf(const std::vector<Value>& arguments);

    /// <summary> Emits a printf call. </summary>
    ///
    /// <param name="format"> Describes the printf format to use. </param>
    /// <param name="arguments"> Arguments to the printf call. </param>
    void Printf(const std::string& format, const std::vector<Value>& arguments);
} // namespace value
} // namespace ell
