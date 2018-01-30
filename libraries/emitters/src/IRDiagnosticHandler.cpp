////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRDiagnosticHandler.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRDiagnosticHandler.h"

// llvm
#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/IR/DiagnosticPrinter.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_os_ostream.h>

// stl
#include <iostream>
#include <sstream>

namespace ell
{
namespace emitters
{
    IRDiagnosticHandler::IRDiagnosticHandler(llvm::LLVMContext& context, bool verbose)
        : _verbose(verbose)
    {
        context.setDiagnosticHandler(&HandleMessage, this);
    }

    void IRDiagnosticHandler::SetVerbosity(bool isVerbose)
    {
        _verbose = isVerbose;
    }

    void IRDiagnosticHandler::HandleMessage(const llvm::DiagnosticInfo& info, void* context)
    {
        if (context == nullptr)
        {
            return;
        }

        IRDiagnosticHandler* handler = static_cast<IRDiagnosticHandler*>(context);
        handler->HandleMessageImpl(info);
    }

    void IRDiagnosticHandler::HandleMessageImpl(const llvm::DiagnosticInfo& info)
    {
        auto severity = info.getSeverity();
        switch (severity)
        {
        case llvm::DS_Error:
            _hadError = true;
            break;

        case llvm::DS_Warning:
        case llvm::DS_Remark:
        case llvm::DS_Note:
        default:
            // nothing
            break;
        }

        const char* prefix = llvm::LLVMContext::getDiagnosticMessagePrefix(severity);
        std::string prefixString(prefix);
        _messagePrefixes.push_back(prefixString);

        llvm::SmallVector<char, 0> buffer;
        llvm::raw_svector_ostream bufferedStream(buffer);
        {
            llvm::DiagnosticPrinterRawOStream printer(bufferedStream);
            info.print(printer);
        }
        std::stringstream stream;
        llvm::raw_os_ostream out(stream);
        out << buffer;
        auto message = stream.str();
        _messages.push_back(message);

        if (_verbose)
        {
            std::cerr << message;
        }
    }
}
}
