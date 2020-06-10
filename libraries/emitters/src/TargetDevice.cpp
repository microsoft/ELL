////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TargetDevice.cpp (emitters)
//  Authors:  Lisa Ong, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TargetDevice.h"
#include "EmitterException.h"
#include "IRAssemblyWriter.h" // for OutputRelocationModel
#include "LLVMUtilities.h"

#include <llvm/ADT/Triple.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

#include <map>

namespace ell
{
namespace emitters
{
    namespace
    {
        static const size_t c_defaultNumBits = 64;

        // Triples
        std::string c_macTriple = "x86_64-apple-macosx10.12.0"; // alternate: "x86_64-apple-darwin16.0.0"
        std::string c_linuxTriple = "x86_64-pc-linux-gnu";
        std::string c_windowsTriple = "x86_64-pc-win32";
        std::string c_armv6Triple = "armv6--linux-gnueabihf"; // raspberry pi 0
        std::string c_armv7Triple = "armv7--linux-gnueabihf"; // raspberry pi 3 and orangepi0
        std::string c_arm64Triple = "aarch64-unknown-linux-gnu"; // DragonBoard
        std::string c_iosTriple = "aarch64-apple-ios"; // alternates: "arm64-apple-ios7.0.0", "thumbv7-apple-ios7.0"

        // CPUs
        std::string c_pi0Cpu = "arm1136jf-s";
        std::string c_pi3Cpu = "cortex-a53";
        std::string c_orangePi0Cpu = "cortex-a7";

        // clang settings:
        // target=armv7-apple-darwin

        std::string c_macDataLayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128";
        std::string c_linuxDataLayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128";
        std::string c_windowsDataLayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128";
        std::string c_armDataLayout = "e-m:e-p:32:32-i64:64-v128:64:128-a:0:32-n32-S64";
        std::string c_arm64DataLayout = "e-m:e-i64:64-i128:128-n32:64-S128"; // DragonBoard
        std::string c_iosDataLayout = "e-m:o-i64:64-i128:128-n32:64-S128";

        const std::map<std::string, std::function<void(TargetDevice&)>> KnownTargetDeviceNameMap = {
            { "mac", [](TargetDevice& targetDevice) {
                 targetDevice.triple = c_macTriple;
                 targetDevice.dataLayout = c_macDataLayout;
             } },
            { "linux", [](TargetDevice& targetDevice) {
                 targetDevice.triple = c_linuxTriple;
                 targetDevice.dataLayout = c_linuxDataLayout;
             } },
            { "windows", [](TargetDevice& targetDevice) {
                 targetDevice.triple = c_windowsTriple;
                 targetDevice.dataLayout = c_windowsDataLayout;
             } },
            { "pi0", [](TargetDevice& targetDevice) {
                 targetDevice.triple = c_armv6Triple;
                 targetDevice.dataLayout = c_armDataLayout;
                 targetDevice.numBits = 32;
                 targetDevice.cpu = c_pi0Cpu; // maybe not necessary
             } },
            { "pi3", [](TargetDevice& targetDevice) {
                 targetDevice.triple = c_armv7Triple;
                 targetDevice.dataLayout = c_armDataLayout;
                 targetDevice.numBits = 32;
                 targetDevice.cpu = c_pi3Cpu; // maybe not necessary
             } },
            { "orangepi0" /* orangepi (Raspbian) */, [](TargetDevice& targetDevice) {
                 targetDevice.triple = c_armv7Triple;
                 targetDevice.dataLayout = c_armDataLayout;
                 targetDevice.numBits = 32;
                 targetDevice.cpu = c_orangePi0Cpu; // maybe not necessary
             } },
            { "pi3_64" /* pi3 (openSUSE) */, [](TargetDevice& targetDevice) {
                 // need to set arch to aarch64?
                 targetDevice.triple = c_arm64Triple;
                 targetDevice.dataLayout = c_arm64DataLayout;
                 targetDevice.numBits = 64;
                 targetDevice.cpu = c_pi3Cpu;
             } },
            { "aarch64" /* arm64 linux (DragonBoard) */, [](TargetDevice& targetDevice) {
                 // need to set arch to aarch64?
                 targetDevice.triple = c_arm64Triple;
                 targetDevice.dataLayout = c_arm64DataLayout;
                 targetDevice.numBits = 64;
             } },
            { "ios", [](TargetDevice& targetDevice) {
                 targetDevice.triple = c_iosTriple;
                 targetDevice.dataLayout = c_iosDataLayout;
             } }
        };

        const std::map<std::string, std::function<void(TargetDevice&)>> KnownTargetDeviceCpuMap = {
            { "cortex-m0", [](TargetDevice& targetDevice) {
                 targetDevice.triple = "armv6m-unknown-none-eabi";
                 targetDevice.features = "+armv6-m,+v6m";
                 targetDevice.architecture = "thumb";
             } },
            { "cortex-m4", [](TargetDevice& targetDevice) {
                 targetDevice.triple = "arm-none-eabi";
                 if (targetDevice.features.empty())
                 {
                     targetDevice.features = "+armv7e-m,+v7,soft-float";
                 }
                 targetDevice.architecture = "arm";
             } }
        };

        llvm::Triple GetNormalizedTriple(std::string tripleString)
        {
            auto normalizedTriple = llvm::Triple::normalize(tripleString.empty() ? llvm::sys::getDefaultTargetTriple() : tripleString);
            return llvm::Triple(normalizedTriple);
        }

        // Function prototypes used internally
        void SetHostTargetProperties(TargetDevice& targetDevice);
        bool HasKnownDeviceName(TargetDevice& targetDevice);
        void SetTargetPropertiesFromName(TargetDevice& targetDevice);
        void VerifyCustomTargetProperties(TargetDevice& targetDevice);
        void SetTargetPropertiesFromCpu(TargetDevice& targetDevice);
        void SetTargetDataLayout(TargetDevice& targetDevice);
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

    TargetDevice GetTargetDevice(std::string deviceName)
    {
        TargetDevice target;
        target.deviceName = deviceName;
        CompleteTargetDevice(target);
        return target;
    }

    void CompleteTargetDevice(TargetDevice& targetDevice)
    {
        auto deviceName = targetDevice.deviceName;

        if (targetDevice.numBits == 0)
        {
            targetDevice.numBits = c_defaultNumBits;
        }

        // Set low-level args based on target name (if present)
        if (deviceName == "host")
        {
            SetHostTargetProperties(targetDevice);
        }
        else if (HasKnownDeviceName(targetDevice))
        {
            SetTargetPropertiesFromName(targetDevice);
        }
        else if (deviceName == "custom")
        {
            SetTargetPropertiesFromCpu(targetDevice);
            SetTargetDataLayout(targetDevice);
            VerifyCustomTargetProperties(targetDevice);
        }
        else
        {
            throw EmitterException(EmitterError::targetNotSupported, "Unknown target device name: " + deviceName);
        }
    }

    namespace
    {
        void SetTargetDataLayout(TargetDevice& targetDevice)
        {
            std::string error;
            const llvm::Target* target = llvm::TargetRegistry::lookupTarget(targetDevice.triple, error);
            if (target == nullptr)
            {
                throw EmitterException(EmitterError::targetNotSupported, "Couldn't create target " + error);
            }
            const OutputRelocationModel relocModel = OutputRelocationModel::Static;
            const llvm::CodeModel::Model codeModel = llvm::CodeModel::Small;
            const llvm::TargetOptions options;
            std::unique_ptr<llvm::TargetMachine> targetMachine(target->createTargetMachine(targetDevice.triple,
                                                                                           targetDevice.cpu,
                                                                                           targetDevice.features,
                                                                                           options,
                                                                                           relocModel,
                                                                                           codeModel,
                                                                                           llvm::CodeGenOpt::Level::Default));

            if (!targetMachine)
            {
                throw EmitterException(EmitterError::targetNotSupported, "Unable to allocate target machine");
            }

            llvm::DataLayout dataLayout(targetMachine->createDataLayout());
            targetDevice.dataLayout = dataLayout.getStringRepresentation();
        }

        void SetHostTargetProperties(TargetDevice& targetDevice)
        {
            InitializeLLVM();

            auto hostTripleString = llvm::sys::getProcessTriple();
            llvm::Triple hostTriple(hostTripleString);

            targetDevice.triple = hostTriple.normalize();
            targetDevice.architecture = llvm::Triple::getArchTypeName(hostTriple.getArch());
            targetDevice.cpu = llvm::sys::getHostCPUName();

            llvm::StringMap<bool> features;
            llvm::sys::getHostCPUFeatures(features);
            for (const auto& feature : features)
            {
                if (feature.second)
                {
                    targetDevice.features += '+' + feature.first().str() + ",";
                }
            }
            if (!targetDevice.features.empty())
            {
                targetDevice.features.pop_back();
            }

            SetTargetDataLayout(targetDevice);
        }

        bool HasKnownDeviceName(TargetDevice& targetDevice)
        {
            auto deviceName = targetDevice.deviceName;
            return (KnownTargetDeviceNameMap.find(deviceName) != KnownTargetDeviceNameMap.end());
        }

        void SetTargetPropertiesFromName(TargetDevice& targetDevice)
        {
            auto deviceName = targetDevice.deviceName;
            auto it = KnownTargetDeviceNameMap.find(deviceName);
            if (it != KnownTargetDeviceNameMap.end())
            {
                (it->second)(targetDevice);
            }
        }

        void VerifyCustomTargetProperties(TargetDevice& targetDevice)
        {
            if (targetDevice.triple == "")
            {
                throw EmitterException(EmitterError::badFunctionArguments, "Missing 'triple' information");
            }
            if (targetDevice.cpu == "")
            {
                throw EmitterException(EmitterError::badFunctionArguments, "Missing 'cpu' information");
            }
        }

        void SetTargetPropertiesFromCpu(TargetDevice& targetDevice)
        {
            auto cpu = targetDevice.cpu;
            auto it = KnownTargetDeviceCpuMap.find(cpu);
            if (it != KnownTargetDeviceCpuMap.end())
            {
                (it->second)(targetDevice);
            }
        }
    } // namespace
} // namespace emitters
} // namespace ell
