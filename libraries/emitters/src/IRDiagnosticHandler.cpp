////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRDiagnosticHandler.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRDiagnosticHandler.h"

#include "llvm/IR/DiagnosticInfo.h"

namespace ell
{
namespace emitters
{
    IRDiagnosticHandler::IRDiagnosticHandler(llvm::LLVMContext& context)
    {
        context.setDiagnosticHandler(&HandleMessage, this);
    }

    void IRDiagnosticHandler::HandleMessage(const llvm::DiagnosticInfo& info, void* context)
    {
        if (context == nullptr)
        {
            return;
        }

        IRDiagnosticHandler* handler = static_cast<IRDiagnosticHandler*>(context);
        auto severity = info.getSeverity();
        if (severity == llvm::DS_Error)
        {
            handler->_hadError = true;
        }

        handler->_messagePrefixes.push_back(llvm::LLVMContext::getDiagnosticMessagePrefix(severity));
    }
}
}
