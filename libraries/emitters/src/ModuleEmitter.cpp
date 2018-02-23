////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModuleEmitter.cpp (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModuleEmitter.h"
#include "EmitterException.h"

// utilities
#include "Files.h"

// llvm
#include <llvm/ADT/Triple.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h> // for CodeGenFileType

// stl
#include <cassert>

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
        std::string c_pi0Triple = "arm-linux-gnueabihf"; // was "armv6m-unknown-none-eabi"
        std::string c_armTriple = "armv7-linux-gnueabihf"; // raspberry pi 3 and orangepi0
        std::string c_arm64Triple = "aarch64-unknown-linux-gnu"; // DragonBoard
        std::string c_iosTriple = "aarch64-apple-ios"; // alternates: "arm64-apple-ios7.0.0", "thumbv7-apple-ios7.0"

        // CPUs
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

        static const std::string c_literalVar = "c_";
        static const std::string c_globalVar = "g_";
        static const std::string c_localVar = "t_";
        static const std::string c_fnVar = "Fn";
        static const std::string c_inputVar = "input";
        static const std::string c_outputVar = "output";
    }

    ModuleEmitter::ModuleEmitter()
    {
    }

    // Sets the parameters (and rationalizes them)
    void ModuleEmitter::SetCompilerOptions(const CompilerOptions& options)
    {
        _options = options;
        CompleteCompilerOptions(_options);
    }

    void ModuleEmitter::CompleteCompilerOptions(CompilerOptions& parameters)
    {
        if (parameters.targetDevice.numBits == 0)
        {
            parameters.targetDevice.numBits = c_defaultNumBits;
        }

        // Set low-level args based on target name (if present)
        if (parameters.targetDevice.deviceName != "")
        {
            if (parameters.targetDevice.deviceName == "host")
            {
                auto hostTripleString = llvm::sys::getProcessTriple();
                llvm::Triple hostTriple(hostTripleString);

                parameters.targetDevice.triple = hostTriple.normalize();
                parameters.targetDevice.architecture = llvm::Triple::getArchTypeName(hostTriple.getArch());
                parameters.targetDevice.cpu = llvm::sys::getHostCPUName();

                std::string error;
                const llvm::Target* target = llvm::TargetRegistry::lookupTarget(parameters.targetDevice.triple, error);
                if (target == nullptr)
                {
                    throw EmitterException(EmitterError::targetNotSupported, std::string("Couldn't create target ") + error);
                }
                
                const llvm::TargetOptions options;
                const llvm::Reloc::Model relocModel = llvm::Reloc::Static;
                const llvm::CodeModel::Model codeModel = llvm::CodeModel::Default;
                std::unique_ptr<llvm::TargetMachine> targetMachine(target->createTargetMachine(parameters.targetDevice.triple,
                                                                                               parameters.targetDevice.cpu,
                                                                                               parameters.targetDevice.features,
                                                                                               options,
                                                                                               relocModel,
                                                                                               codeModel,
                                                                                               llvm::CodeGenOpt::Level::Default));
        
                if (!targetMachine)
                {
                    throw EmitterException(EmitterError::targetNotSupported, "Unable to allocate host target machine");
                }

                assert(targetMachine);
                llvm::DataLayout dataLayout(targetMachine->createDataLayout());
                parameters.targetDevice.dataLayout = dataLayout.getStringRepresentation();
            }
            else if (parameters.targetDevice.deviceName == "mac")
            {
                parameters.targetDevice.triple = c_macTriple;
                parameters.targetDevice.dataLayout = c_macDataLayout;
            }
            else if (parameters.targetDevice.deviceName == "linux")
            {
                parameters.targetDevice.triple = c_linuxTriple;
                parameters.targetDevice.dataLayout = c_linuxDataLayout;
            }
            else if (parameters.targetDevice.deviceName == "windows")
            {
                parameters.targetDevice.triple = c_windowsTriple;
                parameters.targetDevice.dataLayout = c_windowsDataLayout;
            }
            else if (parameters.targetDevice.deviceName == "pi0")
            {
                parameters.targetDevice.triple = c_pi0Triple;
                parameters.targetDevice.dataLayout = c_armDataLayout;
                parameters.targetDevice.numBits = 32;
            }
            else if (parameters.targetDevice.deviceName == "pi3") // pi3 (Raspbian)
            {
                parameters.targetDevice.triple = c_armTriple;
                parameters.targetDevice.dataLayout = c_armDataLayout;
                parameters.targetDevice.numBits = 32;
                parameters.targetDevice.cpu = c_pi3Cpu; // maybe not necessary
            }
            else if (parameters.targetDevice.deviceName == "orangepi0") // orangepi (Raspbian)
            {
                parameters.targetDevice.triple = c_armTriple;
                parameters.targetDevice.dataLayout = c_armDataLayout;
                parameters.targetDevice.numBits = 32;
                parameters.targetDevice.cpu = c_orangePi0Cpu; // maybe not necessary
            }
            else if (parameters.targetDevice.deviceName == "pi3_64") // pi3 (openSUSE)
            {
                // need to set arch to aarch64?
                parameters.targetDevice.triple = c_arm64Triple;
                parameters.targetDevice.dataLayout = c_arm64DataLayout;
                parameters.targetDevice.numBits = 64;
                parameters.targetDevice.cpu = c_pi3Cpu;
            }
            else if (parameters.targetDevice.deviceName == "aarch64") // arm64 linux (DragonBoard)
            {
                // need to set arch to aarch64?
                parameters.targetDevice.triple = c_arm64Triple;
                parameters.targetDevice.dataLayout = c_arm64DataLayout;
                parameters.targetDevice.numBits = 64;
            }
            else if (parameters.targetDevice.deviceName == "ios")
            {
                parameters.targetDevice.triple = c_iosTriple;
                parameters.targetDevice.dataLayout = c_iosDataLayout;
            }
            else if (parameters.targetDevice.deviceName == "custom")
            {
                // perhaps it is a custom target where triple and cpu were set manually.
                if (parameters.targetDevice.triple == "")
                {
                    throw EmitterException(EmitterError::badFunctionArguments, "Missing 'triple' information");
                }
                if (parameters.targetDevice.cpu == "")
                {
                    throw EmitterException(EmitterError::badFunctionArguments, "Missing 'cpu' information");
                }
            }
            else 
            {
                throw EmitterException(EmitterError::targetNotSupported, std::string("Unknown target device name: " + parameters.targetDevice.deviceName));
            }
        }
        else
        {
            if (parameters.targetDevice.cpu == "cortex-m0")
            {
                parameters.targetDevice.triple = "armv6m-unknown-none-eabi";
                parameters.targetDevice.features = "+armv6-m,+v6m";
                parameters.targetDevice.architecture = "thumb";
            }
            else if (parameters.targetDevice.cpu == "cortex-m4")
            {
                parameters.targetDevice.triple = "arm-none-eabi";
                parameters.targetDevice.features = "+armv7e-m,+v7,soft-float";
            }
        }
    }

    void ModuleEmitter::WriteToFile(const std::string& filePath)
    {
        WriteToFile(filePath, GetFormatFromExtension(utilities::GetFileExtension(filePath, true)));
    }

    void ModuleEmitter::WriteToFile(const std::string& filePath, ModuleOutputFormat format)
    {
        auto stream = utilities::OpenOfstream(filePath);
        WriteToStream(stream, format);
    }

    ModuleOutputFormat ModuleEmitter::GetFormatFromExtension(const std::string& extension)
    {
        if (extension == "ll")
        {
            return ModuleOutputFormat::ir;
        }
        else if (extension == "bc")
        {
            return ModuleOutputFormat::bitcode;
        }
        else if (extension == "s" || extension == "asm")
        {
            return ModuleOutputFormat::assembly;
        }
        else if (extension == "o" || extension == "obj")
        {
            return ModuleOutputFormat::objectCode;
        }
        else if (extension == "h")
        {
            return ModuleOutputFormat::cHeader;
        }
        else if (extension == "i")
        {
            return ModuleOutputFormat::swigInterface;
        }
        else
        {
            return ModuleOutputFormat::ir; // default
        }
    }

    void ModuleEmitter::Reset()
    {
    }

    void ModuleEmitter::AllocateVariable(Variable& var)
    {
        if (var.HasEmittedName())
        {
            return;
        }

        EmittedVariable emittedVar;
        const std::string* pPrefix = nullptr;
        switch (var.Scope())
        {
            case VariableScope::literal:
                emittedVar = _literalVars.Allocate();
                pPrefix = &c_literalVar;
                break;
            case VariableScope::local:
                emittedVar = _localVars.Allocate();
                pPrefix = &c_localVar;
                break;
            case VariableScope::global:
                emittedVar = _globalVars.Allocate();
                pPrefix = &c_globalVar;
                break;
            case VariableScope::rValue:
                emittedVar = _rValueVars.Allocate();
                pPrefix = &c_fnVar;
                break;
            case VariableScope::input:
                emittedVar = _inputVars.Allocate();
                pPrefix = &c_inputVar;
                break;
            case VariableScope::output:
                emittedVar = _outputVars.Allocate();
                pPrefix = &c_outputVar;
                break;
            default:
                throw EmitterException(EmitterError::variableScopeNotSupported);
        }
        var.AssignVariable(emittedVar);
        var.SetEmittedName(*pPrefix + std::to_string(emittedVar.varIndex));
    }

    void ModuleEmitter::FreeVariable(Variable& var)
    {
        if (!var.HasEmittedName())
        {
            return;
        }
        VariableScope scope = var.Scope();
        switch (scope)
        {
            case VariableScope::local:
            {
                auto v = var.GetAssignedVar();
                _localVars.Free(v);
            }
            break;
            case VariableScope::global:
            {
                auto v = var.GetAssignedVar();
                _globalVars.Free(v);
            }
            break;
            default:
                // We never free other types
                assert(scope != VariableScope::local && scope != VariableScope::global);
                break;
        }
    }
}
}
