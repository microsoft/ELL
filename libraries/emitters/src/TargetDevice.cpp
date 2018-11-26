////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TargetDevice.cpp (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TargetDevice.h"

#include <llvm/ADT/Triple.h>
#include <llvm/Support/Host.h>

namespace ell
{
namespace emitters
{
    namespace
    {
        llvm::Triple GetNormalizedTriple(std::string tripleString)
        {
            auto normalizedTriple = llvm::Triple::normalize(tripleString.empty() ? llvm::sys::getDefaultTargetTriple() : tripleString);
            return llvm::Triple(normalizedTriple);
        }
    } // namespace

    bool TargetDevice::IsWindows() const
    {
        auto tripleObj = GetNormalizedTriple(triple);
        return tripleObj.getOS() == llvm::Triple::Win32;
    }

    bool TargetDevice::IsLinux() const
    {
        auto tripleObj = GetNormalizedTriple(triple);
        return tripleObj.getOS() == llvm::Triple::Linux;
    }

    bool TargetDevice::IsMacOS() const
    {
        auto tripleObj = GetNormalizedTriple(triple);
        return tripleObj.getOS() == llvm::Triple::MacOSX || tripleObj.getOS() == llvm::Triple::Darwin;
    }
} // namespace emitters
} // namespace ell
