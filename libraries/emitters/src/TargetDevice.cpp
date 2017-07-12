////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TargetDevice.cpp (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TargetDevice.h"
#include "LLVMInclude.h"

namespace ell
{
namespace emitters
{
    bool TargetDevice::IsWindows() const
    {
        // Infer the triple from LLVM if not set
        auto t = triple.empty() ? llvm::sys::getDefaultTargetTriple() : triple;
        return (t == "x86_64-pc-win32" || t == "x86_64-pc-windows-msvc" ||
                t == "i386-pc-win32" || t == "i386-pc-windows-msvc");
    }
}
}
