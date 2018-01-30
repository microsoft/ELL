////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLoader.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRLoader.h"
#include "EmitterException.h"

// llvm
#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/raw_os_ostream.h>

// stl
#include <sstream>

namespace ell
{
namespace emitters
{
    std::unique_ptr<llvm::Module> IRLoader::LoadAssembly(const std::string& text)
    {
        llvm::SMDiagnostic error;
        std::unique_ptr<llvm::Module> pModule = llvm::parseAssemblyString(text, error, _llvmContext);
        if (!pModule)
        {
            throw EmitterException(EmitterError::parserError, ErrorToString(error));
        }
        return pModule;
    }

    std::unique_ptr<llvm::Module> IRLoader::LoadAssemblyFile(const std::string& filePath)
    {
        llvm::SMDiagnostic error;
        std::unique_ptr<llvm::Module> pModule = llvm::parseAssemblyFile(filePath, error, _llvmContext);
        if (!pModule)
        {
            throw EmitterException(EmitterError::parserError, ErrorToString(error));
        }
        return pModule;
    }

    std::string IRLoader::ErrorToString(llvm::SMDiagnostic error)
    {
        std::stringstream buffer;
        llvm::raw_os_ostream ostream(buffer);
        error.print(nullptr, ostream, false, true);
        return buffer.str();
    }
}
}
