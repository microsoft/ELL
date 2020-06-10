////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMContext.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ComputeContext.h"
#include "EmitterContext.h"
#include "FunctionDeclaration.h"
#include "Scalar.h"

#include <emitters/include/IRModuleEmitter.h>
#include <emitters/include/LLVMUtilities.h>

#include <functional>
#include <memory>
#include <stack>

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;
} // namespace emitters
} // namespace ell

namespace ell
{
namespace value
{
    /// <summary> A specialization of EmitterContext that emits LLVM IR </summary>
    class LLVMContext : public EmitterContext
    {
    public:
        /// <summary> Constructor </summary>
        /// <param name="emitter"> A reference to an IRModuleEmitter that will be used to emit LLVM IR </param>
        explicit LLVMContext(emitters::IRModuleEmitter& emitter);

        /// <summary> Constructor </summary>
        /// <param name="emitter"> Takes ownership of the IRModuleEmitter that will be used to emit LLVM IR </param>
        explicit LLVMContext(std::unique_ptr<emitters::IRModuleEmitter>&& emitter);

        /// <summary> Constructor </summary>
        ///
        /// <param name="moduleName"> Name of the module. </param>
        /// <param name="parameters"> Options for the compiler </param>
        LLVMContext(const std::string& moduleName, const emitters::CompilerOptions& parameters);

        emitters::IRModuleEmitter& GetModuleEmitter() const;

        emitters::IRFunctionEmitter& GetFunctionEmitter() const;

        emitters::LLVMFunction DeclareFunction(const FunctionDeclaration& func);

        std::optional<emitters::LLVMValue> ToLLVMValue(Value value) const;
        std::vector<std::optional<emitters::LLVMValue>> ToLLVMValue(std::vector<Value> values) const;

        emitters::LLVMValue ToLLVMValue(Value value);
        std::vector<emitters::LLVMValue> ToLLVMValue(std::vector<Value> values);

    private:
        Value AllocateImpl(ValueType value, MemoryLayout layout, size_t alignment, AllocateFlags flags = AllocateFlags::None) override;

        std::optional<Value> GetGlobalValue(GlobalAllocationScope scope, std::string name) override;

        Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ConstantData data, MemoryLayout layout, AllocateFlags flags = AllocateFlags::None) override;
        Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ValueType type, MemoryLayout layout, AllocateFlags flags = AllocateFlags::None) override;

        detail::ValueTypeDescription GetTypeImpl(Emittable emittable) override;

        DefinedFunction CreateFunctionImpl(FunctionDeclaration decl, DefinedFunction fn) override;
        bool IsFunctionDefinedImpl(FunctionDeclaration decl) const override;

        Value StoreConstantDataImpl(ConstantData data) override;

        void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn, const std::string& name) override;
        void ForImpl(Scalar start, Scalar stop, Scalar step, std::function<void(Scalar)> fn, const std::string& name) override;

        void MoveDataImpl(Value& source, Value& destination) override;

        void CopyDataImpl(const Value& source, Value& destination) override;

        Value ReferenceImpl(Value source) override;

        Value DereferenceImpl(Value source) override;

        Value OffsetImpl(Value begin, Value offset) override;

        Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) override;
        Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) override;

        Value LogicalOperationImpl(ValueLogicalOperation op, Value source1, Value source2) override;

        Value CastImpl(Value value, ValueType type) override;

        IfContext IfImpl(Scalar test, std::function<void()> fn) override;

        void WhileImpl(Scalar test, std::function<void()> fn) override;

        std::optional<Value> CallImpl(FunctionDeclaration func, std::vector<Value> args) override;

        void PrefetchImpl(Value data, PrefetchType type, PrefetchLocality locality) override;

        void ParallelizeImpl(int numTasks, std::vector<Value> captured, std::function<void(Scalar, std::vector<Value>)> fn) override;

        void DebugBreakImpl() override;
        void DebugDumpImpl(Value value, std::string tag, std::ostream& stream) const override;
        void DebugDumpImpl(FunctionDeclaration fn, std::string tag, std::ostream& stream) const override;
        void DebugPrintImpl(std::string message) override;

        void SetNameImpl(const Value& value, const std::string& name) override;
        std::string GetNameImpl(const Value& value) const override;

        void ImportCodeFileImpl(std::string) override;

        Scalar GetFunctionAddressImpl(const FunctionDeclaration& fn) override;

        Value IntrinsicCall(FunctionDeclaration intrinsic, std::vector<Value> args);

        std::optional<Value> EmitExternalCall(FunctionDeclaration func, std::vector<Value> args);

        bool TypeCompatible(Value value1, Value value2);

        std::string GetScopeAdjustedName(GlobalAllocationScope scope, std::string name) const;
        std::string GetGlobalScopedName(std::string name) const;
        std::string GetCurrentFunctionScopedName(std::string name) const;

        emitters::LLVMFunctionType ToLLVMFunctionType(const FunctionDeclaration& func) const;

        struct PromotedConstantDataDescription
        {
            const ConstantData* data;
            Emittable realValue;
        };

        Value PromoteConstantData(Value value);
        std::optional<PromotedConstantDataDescription> HasBeenPromoted(Value value) const;
        Value Realize(Value value) const;
        Value EnsureEmittable(Value value);
        std::vector<Value> EnsureEmittable(std::vector<Value> values);

        class IfContextImpl;
        struct FunctionScope;

        std::stack<std::vector<PromotedConstantDataDescription>> _promotedConstantStack;

        std::unique_ptr<emitters::IRModuleEmitter> _ownedEmitter;
        emitters::IRModuleEmitter& _emitter;

        // LLVMContext uses ComputeContext internally to handle cases where all relevant operands are constant
        ComputeContext _computeContext;

        std::stack<std::reference_wrapper<emitters::IRFunctionEmitter>> _functionStack;
        std::map<std::string, std::pair<Emittable, MemoryLayout>> _globals;
        std::unordered_map<FunctionDeclaration, DefinedFunction> _definedFunctions;
    };

    emitters::LLVMValue ToLLVMValue(Value value);
    emitters::LLVMValue ToLLVMValue(ViewAdapter value);

    std::vector<emitters::LLVMValue> ToLLVMValue(std::vector<Value> values);
} // namespace value
} // namespace ell
