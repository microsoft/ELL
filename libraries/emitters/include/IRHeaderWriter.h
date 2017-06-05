////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRHeaderWriter.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IRModuleEmitter.h"

// stl
#include <ostream>

namespace ell
{
namespace emitters
{
    /// <summary> Write a C / C++ header file for the given module. </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to write </param>
    void WriteModuleHeader(std::ostream& os, IRModuleEmitter& moduleEmitter);
}
}
