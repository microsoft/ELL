////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRAssemblyEmitter.h (emitters)
//  Authors:
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// llvm
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CodeGen.h" // for CodeGenOpt::Level enum
#include "llvm/Target/TargetMachine.h" // for CodeGenFileType
#include "llvm/Target/TargetOptions.h" // for FloatABI::ABIType and FPOpFusion::FpOpFusionMode

// stl
#include <ostream>

namespace ell
{
namespace emitters
{
    /// <summary> An enum containing the optimization level {None, Less, Default, Aggressive} </summary>
    typedef llvm::CodeGenOpt::Level OptimizationLevel;

    /// <summary> An enum containing the float ABI type {Default, Soft, Hard} </summary>
    typedef llvm::FloatABI::ABIType FloatABIType;

    /// <summary> An enum indicating how much floating-point operations can be fused {Fast, Standard, Strict} </summary>
    typedef llvm::FPOpFusion::FPOpFusionMode FloatFusionMode;

    /// <summary> An enum containing the type of output to generate {CGFT_AssemblyFile, CGFT_ObjectFile, CGFT_Null} </summary>
    typedef llvm::TargetMachine::CodeGenFileType OutputFileType;

    /// <summary> Options for the LLVM -> assembly compilation </summary>
    struct MachineCodeOutputOptions
    {
        bool verboseOutput = false;
        bool verifyModule = false;
        std::string triple = "armv6m-unknown-none-eabi";
        std::string architecture = "thumb";
        std::string cpu = "cortex-m0";
        std::string targetFeatures = "+armv6-m,+v6m";
        OptimizationLevel optimizationLevel = OptimizationLevel::Default;
        FloatABIType floatABI = FloatABIType::Default;
        FloatFusionMode floatFusionMode = FloatFusionMode::Standard;
    };

    /// <summary> Indicates if the requested output type is binary or text </summary>
    bool IsBinaryOutputType(const OutputFileType& filetype);

    /// <summary> Compile the given module to the given stream </summary>
    void GenerateMachineCode(llvm::raw_ostream& os, llvm::Module& module, OutputFileType fileType, const MachineCodeOutputOptions& options);
}
}
