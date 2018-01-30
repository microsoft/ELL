////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRHeaderWriter.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// llvm
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

// stl
#include <ostream>
#include <string>

namespace ell
{
namespace emitters
{
    class IRModuleEmitter;

    /// <summary> Write a C / C++ header file for the given module. </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to write </param>
    void WriteModuleHeader(std::ostream& os, IRModuleEmitter& moduleEmitter);

    /// <summary> Writes a C++ wrapper for the given module. </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to write </param>
    void WriteModuleCppWrapper(std::ostream& os, IRModuleEmitter& moduleEmitter);

    /// <summary> Writes the declaration for a function </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to write </param>
    /// <param name="function"> Pointer to the llvm function. </param>
    void WriteFunctionDeclaration(std::ostream& os, IRModuleEmitter& moduleEmitter, llvm::Function& function);

    /// <summary> Writes an LLVM type. </summary>
    ///
    /// <param name="os"> The output stream to write to. </param>
    /// <param name="type"> Pointer to the llvm type. </param>
    void WriteLLVMType(std::ostream& os, llvm::Type* t);

    /// <summary> Globally searches and replaces a delimiter in a given text. </summary>
    ///
    /// <param name="text"> The text to process. </param>
    /// <param name="delimiter"> The delimiter to replace. </param>
    /// <param name="replacement"> The replacement text. </param>
    void ReplaceDelimiter(std::string& text, const std::string& delimiter, const std::string& replacement);

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
        /// <param name="value"> The value to assign to the aforementioned symbol. Empty by default. </param>
        DeclareIfDefDefine(std::ostream& os, std::string symbol, const std::string& value = "");
    };

    struct FunctionTagValues;

    /// <summary> Holds callback definitions for a module. </summary>
    struct ModuleCallbackDefinitions
    {
        /// <summary> Constructor. </summary>
        ///
        /// <param name="callbacks"> List of llvm functions and tag values. </param>
        ModuleCallbackDefinitions(const std::vector<FunctionTagValues>& callbacks);

        struct CallbackSignature
        {
            /// <summary> Constructor. </summary>
            ///
            /// <param name="function"> Pointer to the callback's llvm function. </param>
            CallbackSignature(llvm::Function& function);

            std::string functionName;
            std::string className;
            std::string inputType;
            std::string returnType;
            bool inputIsScalar;
        };

        std::vector<CallbackSignature> sources;
        std::vector<CallbackSignature> sinks;
        std::vector<CallbackSignature> lagNotifications;
    };
}
}
