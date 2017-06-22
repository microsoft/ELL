////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRSwigInterfaceWriter.h (emitters)
//  Authors:  Lisa Ong
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
    /// <summary> Write a C++ SWIG header file for the given module. </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to write </param>
    void WriteModuleSwigHeader(std::ostream& os, IRModuleEmitter& moduleEmitter);

    /// <summary> Write a C++ SWIG interface file for the given module. </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to write </param>
    /// <param name="headerName"> The name of the SWIG header file </param>
    void WriteModuleSwigInterface(std::ostream& os, IRModuleEmitter& moduleEmitter, const std::string& headerName);
}
}
