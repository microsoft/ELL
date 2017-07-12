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

    /// <summary> Writes the declaration for a function </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to write </param>
    /// <param name="type"> Pointer to the llvm function. </param>
    void WriteFunctionDeclaration(std::ostream& os, IRModuleEmitter& moduleEmitter, llvm::Function& function);

    /// <summary> Writes an LLVM type. </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="type"> Pointer to the llvm type. </param>
    void WriteLLVMType(std::ostream& os, llvm::Type* t);

    //
    // Utility classes
    //
    class DeclareExternC
    {
    public:
        /// <summary> Writes a scoped extern "C" declaration. </summary>
        ///
        /// <param name="os"> The output stream to write to. </param>
        DeclareExternC(std::ostream& os);

        /// <summary> Closes a scoped extern "C" declaration. </summary>
        ~DeclareExternC();

    private:
        std::ostream* _os;
    };
}
}
