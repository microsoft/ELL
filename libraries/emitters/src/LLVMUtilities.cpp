////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMUtilities.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LLVMUtilities.h"
#include "EmitterException.h"

#include "build/LLVMEmitterTargets.h"

#include <llvm/IR/Value.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

namespace ell
{
namespace emitters
{
    //
    // Get types from LLVM values
    //
    LLVMTypeList GetLLVMTypes(const std::vector<LLVMValue>& values)
    {
        LLVMTypeList result;
        result.reserve(values.size());
        for (auto v : values)
        {
            result.push_back(v->getType());
        }
        return result;
    }

    emitters::TypedOperator GetOperator(LLVMType type, BinaryOperatorType operation)
    {
        if (type->isIntegerTy() && type->getIntegerBitWidth() == 1)
        {
            return GetBooleanOperator(operation);
        }
        else if (type->isIntegerTy())
        {
            return GetIntegerOperator(operation);
        }
        else if (type->isFloatingPointTy())
        {
            return GetFloatOperator(operation);
        }

        throw EmitterException(EmitterError::valueTypeNotSupported);
    }

    emitters::TypedComparison GetComparison(LLVMType type, BinaryPredicateType comparison)
    {
        if (type->isIntegerTy())
        {
            return GetIntegerComparison(comparison);
        }
        else if (type->isFloatingPointTy())
        {
            return GetFloatComparison(comparison);
        }

        throw EmitterException(EmitterError::valueTypeNotSupported);
    }

    VariableType ToVariableType(LLVMType type)
    {
        if (type->isPointerTy())
        {
            auto et = type->getPointerElementType();
            switch (ToVariableType(et))
            {
            case VariableType::Void:
                return VariableType::VoidPointer;
            case VariableType::Double:
                return VariableType::DoublePointer;
            case VariableType::Float:
                return VariableType::FloatPointer;
            case VariableType::Byte:
                return VariableType::BytePointer;
            case VariableType::Int16:
                return VariableType::Int16Pointer;
            case VariableType::Int32:
                return VariableType::Int32Pointer;
            case VariableType::Int64:
                return VariableType::Int64Pointer;
            default:
                break;
            }
        }
        else
        {
            if (type->isVoidTy())
            {
                return VariableType::Void;
            }
            else if (type->isDoubleTy())
            {
                return VariableType::Double;
            }
            else if (type->isFloatTy())
            {
                return VariableType::Float;
            }
            else if (type->isIntegerTy())
            {
                switch (type->getIntegerBitWidth())
                {
                case 8:
                    return VariableType::Byte;
                case 16:
                    return VariableType::Int16;
                case 32:
                    return VariableType::Int32;
                case 64:
                    return VariableType::Int64;
                }
            }
        }
        return VariableType::Custom;
    }

    namespace
    {
        void InitializeLLVMTargets()
        {
            // This block is part of a X-Macro. LLVM_EMITTER_TARGETS below is
            // defined in build/LLVMEmitterTargets.h at CMake configure time.
            // It is dependent on the value of the CMake variable LLVM_EMITTER_TARGETS.
            // For each LLVM target specified in that variable, EMITTER_TARGET_ACTION
            // below gets called
#define EMITTER_TARGET_ACTION(TargetName)     \
    LLVMInitialize##TargetName##TargetInfo(); \
    LLVMInitialize##TargetName##Target();     \
    LLVMInitialize##TargetName##TargetMC();   \
    LLVMInitialize##TargetName##AsmPrinter(); \
    LLVMInitialize##TargetName##AsmParser();  \
    LLVMInitialize##TargetName##Disassembler();
            LLVM_EMITTER_TARGETS
#undef EMITTER_TARGET_ACTION

            llvm::InitializeNativeTarget();
        }

        void InitializeGlobalPassRegistry()
        {
            // Get the global pass registry
            llvm::PassRegistry* registry = llvm::PassRegistry::getPassRegistry();

            // Initialize all of the optimization passes (probably unnecessary)
            llvm::initializeCore(*registry);
            llvm::initializeScalarOpts(*registry);
            llvm::initializeVectorization(*registry);
            llvm::initializeIPO(*registry);
            llvm::initializeAnalysis(*registry);
            llvm::initializeTransformUtils(*registry);
            llvm::initializeInstCombine(*registry);
            llvm::initializeAggressiveInstCombine(*registry);
            llvm::initializeInstrumentation(*registry);
            llvm::initializeTarget(*registry);
            llvm::initializeGlobalISel(*registry);

            // For codegen passes, only passes that do IR to IR transformation are
            // supported.
            llvm::initializeExpandMemCmpPassPass(*registry);
            llvm::initializeScalarizeMaskedMemIntrinPass(*registry);
            llvm::initializeCodeGenPreparePass(*registry);
            llvm::initializeAtomicExpandPass(*registry);
            llvm::initializeRewriteSymbolsLegacyPassPass(*registry);
            llvm::initializeWinEHPreparePass(*registry);
            llvm::initializeDwarfEHPreparePass(*registry);
            llvm::initializeSafeStackLegacyPassPass(*registry);
            llvm::initializeSjLjEHPreparePass(*registry);
            llvm::initializePreISelIntrinsicLoweringLegacyPassPass(*registry);
            llvm::initializeGlobalMergePass(*registry);
            llvm::initializeIndirectBrExpandPassPass(*registry);
            llvm::initializeInterleavedLoadCombinePass(*registry);
            llvm::initializeInterleavedAccessPass(*registry);
            llvm::initializeEntryExitInstrumenterPass(*registry);
            llvm::initializePostInlineEntryExitInstrumenterPass(*registry);
            llvm::initializeUnreachableBlockElimLegacyPassPass(*registry);
            llvm::initializeExpandReductionsPass(*registry);
            llvm::initializeWriteBitcodePassPass(*registry);
        }
    } // namespace

    void InitializeLLVM()
    {
        InitializeLLVMTargets();
        InitializeGlobalPassRegistry();
    }

} // namespace emitters
} // namespace ell
