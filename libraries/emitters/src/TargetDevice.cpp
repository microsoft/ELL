////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TargetDevice.cpp (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TargetDevice.h"

// llvm
#include <llvm/ADT/Triple.h>
#include <llvm/Support/Host.h>

namespace ell
{
namespace emitters
{
    bool TargetDevice::IsWindows() const
    {
        llvm::Triple tripleObj(triple.empty() ? llvm::sys::getDefaultTargetTriple() : triple);
        return tripleObj.getOS() == llvm::Triple::Win32;
    }

    bool TargetDevice::IsLinux() const
    {
        llvm::Triple tripleObj(triple.empty() ? llvm::sys::getDefaultTargetTriple() : triple);
        return tripleObj.getOS() == llvm::Triple::Linux;
    }

    bool TargetDevice::IsMacOS() const
    {
        llvm::Triple tripleObj(triple.empty() ? llvm::sys::getDefaultTargetTriple() : triple);
        return tripleObj.getOS() == llvm::Triple::MacOSX || tripleObj.getOS() == llvm::Triple::Darwin;
    }
}
}
