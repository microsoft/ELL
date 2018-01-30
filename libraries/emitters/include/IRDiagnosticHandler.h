////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRDiagnosticHandler.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// llvm
#include <llvm/IR/LLVMContext.h>

// stl
#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    /// <summary> An LLVM diagnostic handler class that collects warning and error codes </summary>
    class IRDiagnosticHandler
    {
    public:
        /// <summary> Indicate if there were errors </summary>
        ///
        /// <returns> true if there was an error </returns>
        bool HadError() const { return _hadError; }

        /// <summary> Gets the warning and error messages </summary>
        ///
        /// <returns> A std::vector of warning and error messages </returns>
        const std::vector<std::string>& GetMessages() const { return _messagePrefixes; }

        /// <summary> Clears any messages that have been collected by the diagnostic handler </summary>
        void ClearMessages() { _messagePrefixes.clear(); }

        /// <summary> Set verbosity of handler. </summary>
        ///
        /// <param name="isVerbose"> If true, set the handler to verbose mode, if false, set the handler to quiet mode. </param>
        void SetVerbosity(bool isVerbose);

    private:
        friend class IRModuleEmitter;
        // friend void GenerateMachineCode(llvm::raw_ostream& os, llvm::Module& module, OutputFileType fileType, const MachineCodeOutputOptions& options);

        IRDiagnosticHandler() = delete;
        IRDiagnosticHandler(IRDiagnosticHandler&) = delete;
        IRDiagnosticHandler(IRDiagnosticHandler&&) = delete;
        IRDiagnosticHandler(llvm::LLVMContext& context, bool verbose = false);

        static void HandleMessage(const llvm::DiagnosticInfo& info, void* context); // callback
        void HandleMessageImpl(const llvm::DiagnosticInfo& info); // implementation
        bool _verbose = false;

        bool _hadError = false;
        std::vector<std::string> _messagePrefixes;
        std::vector<std::string> _messages;
    };
}
}
