////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLoader.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// llvm
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>

// stl
#include <memory>
#include <string>

namespace ell
{
namespace emitters
{
    /// <summary> Assists with loading IR modules, assembly files, etc </summary>
    class IRLoader
    {
    public:
        /// <summary></summary>
        IRLoader(llvm::LLVMContext& context)
            : _llvmContext(context) {}

        /// <summary> Load a module by parsing the given assembly text. </summary>
        ///
        /// <param name="text"> Assembly text. </param>
        ///
        /// <returns> Unique pointer to the module. </returns>
        std::unique_ptr<llvm::Module> LoadAssembly(const std::string& text);

        /// <summary> Load a module assembly from file. </summary>
        ///
        /// <param name="filePath"> Full pathname of the file. </param>
        ///
        /// <returns> Unique pointer to the module. </returns>
        std::unique_ptr<llvm::Module> LoadAssemblyFile(const std::string& filePath);

        /// <summary> Translate the diagnostic information to an error string. </summary>
        ///
        /// <param name="error"> The error. </param>
        ///
        /// <returns> A string that describes the error. </returns>
        static std::string ErrorToString(llvm::SMDiagnostic error);

    private:
        llvm::LLVMContext& _llvmContext;
    };
}
}
