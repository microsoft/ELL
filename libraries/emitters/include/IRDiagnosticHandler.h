////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRDiagnosticHandler.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "llvm/IR/LLVMContext.h"

// stl
#include <vector>

namespace ell
{
namespace emitters
{
    /// <summary> An LLVM diagnostic handler class that collects warning and error codes </summary>
    class IRDiagnosticHandler
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="context"> The LLVM context </summary>
        IRDiagnosticHandler(llvm::LLVMContext& context);

        /// <summary> Indicate if there were errors </summary>
        ///
        /// <returns> true if there was an error </returns>
        bool HadError() { return _hadError; }

        /// <summary> Gets the warning and error messages </summary>
        ///
        /// <returns> A std::vector of warning and error messages </returns>
        std::vector<std::string> GetMessages() { return _messagePrefixes; }

    private:
        static void HandleMessage(const llvm::DiagnosticInfo& info, void* context);
        bool _hadError = false;
        std::vector<std::string> _messagePrefixes;
    };
}
}
