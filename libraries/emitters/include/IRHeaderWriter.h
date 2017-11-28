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

    struct DeclareIfDefGuard
    {
        enum class Type
        {
            Negative,
            Positive
        };

        /// <summary> Writes a scoped preprocessor guard declaration for a symbol that's externally defined. </summary>
        ///
        /// <param name="os"> The output stream to write to. </param>
        /// <param name="symbol"> The preprocessor symbol. </param>
        /// <param name="type"> The type of guard. If positive, the guard checks for the symbol to be defined already. Negative is opposite.
        /// </param>
        DeclareIfDefGuard(std::ostream& os, std::string symbol, Type type);

        /// <summary> Closes a scoped preprocessor guard declaration. </summary>
        ~DeclareIfDefGuard();

    protected:
        std::ostream& _os;
        std::string _symbol;
        Type _type;
    };

    struct DeclareIfDefDefine : private DeclareIfDefGuard
    {
        /// <summary> Writes a scoped preprocessor guard declaration. </summary>
        ///
        /// <param name="os"> The output stream to write to. </param>
        /// <param name="symbol"> The preprocessor symbol. </param>
        /// <param name="value"> The value to assign to the aforementioned symbol. Empty by defualt. </param>
        DeclareIfDefDefine(std::ostream& os, std::string symbol, const std::string& value = "");
    };
}
}
