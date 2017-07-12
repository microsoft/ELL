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

// stl
#include <cassert>

namespace ell
{
namespace emitters
{
    namespace
    {
        static const size_t c_defaultNumBits = 64;

        std::string c_macTriple = "x86_64-apple-macosx10.12.0"; // alternate: "x86_64-apple-darwin16.0.0"
        std::string c_linuxTriple = "x86_64-pc-linux-gnu";
        std::string c_windowsTriple = "x86_64-pc-win32";
        std::string c_armTriple = "armv7-linux-gnueabihf";
        std::string c_arm64Triple = "aarch64-unknown-linux-gnu"; // DragonBoard
        std::string c_iosTriple = "aarch64-apple-ios"; // alternates: "arm64-apple-ios7.0.0", "thumbv7-apple-ios7.0"
        // clang settings:
        // triple=thumbv7-apple-ios7.0
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
        SetCompilerParameters(_parameters);
    }

    // Sets the parameters (and rationalizes them)
    void ModuleEmitter::SetCompilerParameters(const CompilerParameters& parameters)
    {
        _parameters = parameters;

        if (_parameters.targetDevice.numBits == 0)
        {
            _parameters.targetDevice.numBits = c_defaultNumBits;
        }

        // Set low-level args based on target name (if present)
        if (_parameters.targetDevice.deviceName != "")
        {
            if (_parameters.targetDevice.deviceName == "mac")
            {
                _parameters.targetDevice.triple = c_macTriple;
                _parameters.targetDevice.dataLayout = c_macDataLayout;
            }
            else if (_parameters.targetDevice.deviceName == "linux")
            {
                _parameters.targetDevice.triple = c_linuxTriple;
                _parameters.targetDevice.dataLayout = c_linuxDataLayout;
            }
            else if (_parameters.targetDevice.deviceName == "windows")
            {
                _parameters.targetDevice.triple = c_windowsTriple;
                _parameters.targetDevice.dataLayout = c_windowsDataLayout;
            }
            else if (_parameters.targetDevice.deviceName == "pi3") // pi3 (Raspbian)
            {
                _parameters.targetDevice.triple = c_armTriple; // For some reason, the assembly doesn't like this
                _parameters.targetDevice.dataLayout = c_armDataLayout;
                _parameters.targetDevice.numBits = 32;
                _parameters.targetDevice.cpu = "cortex-a53"; // maybe not necessary
            }
            else if (_parameters.targetDevice.deviceName == "pi3_64") // pi3 (openSUSE)
            {
                // need to set arch to aarch64?
                _parameters.targetDevice.triple = c_arm64Triple;
                _parameters.targetDevice.dataLayout = c_arm64DataLayout;
                _parameters.targetDevice.numBits = 64;
                _parameters.targetDevice.cpu = "cortex-a53";
            }
            else if (_parameters.targetDevice.deviceName == "aarch64") // arm64 linux (DragonBoard)
            {
                // need to set arch to aarch64?
                _parameters.targetDevice.triple = c_arm64Triple;
                _parameters.targetDevice.dataLayout = c_arm64DataLayout;
                _parameters.targetDevice.numBits = 64;
            }
            else if (_parameters.targetDevice.deviceName == "ios")
            {
                _parameters.targetDevice.triple = c_iosTriple;
                _parameters.targetDevice.dataLayout = c_iosDataLayout;
            }
            else
            {
                // throw an exception?
            }
        }
        else
        {
            if (_parameters.targetDevice.cpu == "cortex-m0")
            {
                _parameters.targetDevice.triple = "armv6m-unknown-none-eabi";
                _parameters.targetDevice.features = "+armv6-m,+v6m";
                _parameters.targetDevice.architecture = "thumb";
            }
            else if (_parameters.targetDevice.cpu == "cortex-m4")
            {
                _parameters.targetDevice.triple = "arm-none-eabi";
                _parameters.targetDevice.features = "+armv7e-m,+v7,soft-float";
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
        else if (extension == "s" || extension == "asm")
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
