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

#include <functional>
#include <optional>
#include <stack>

namespace ell
{
namespace emitters
{

    class IRFunctionEmitter;
    class IRModuleEmitter;

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
        LLVMContext(emitters::IRModuleEmitter& emitter);

        const emitters::IRModuleEmitter& GetModuleEmitter() const;

        emitters::IRFunctionEmitter& GetFunctionEmitter() const;

    private:
        Value AllocateImpl(ValueType value, MemoryLayout layout) override;

        std::optional<Value> GetGlobalValue(GlobalAllocationScope scope, std::string name) override;

        Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ConstantData data, MemoryLayout layout) override;
        Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ValueType type, MemoryLayout layout) override;

        detail::ValueTypeDescription GetTypeImpl(Emittable emittable) override;

        DefinedFunction CreateFunctionImpl(FunctionDeclaration decl, DefinedFunction fn) override;
        bool IsFunctionDefinedImpl(FunctionDeclaration decl) const override;

        Value StoreConstantDataImpl(ConstantData data) override;

        void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn) override;
        void ForImpl(Scalar start, Scalar stop, Scalar step, std::function<void(Scalar)> fn) override;

        void MoveDataImpl(Value& source, Value& destination) override;

        void CopyDataImpl(const Value& source, Value& destination) override;

        Value OffsetImpl(Value begin, Value offset) override;

        Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) override;
        Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) override;

        Value LogicalOperationImpl(ValueLogicalOperation op, Value source1, Value source2) override;

        Value CastImpl(Value value, ValueType type) override;

        IfContext IfImpl(Scalar test, std::function<void()> fn) override;

        std::optional<Value> CallImpl(FunctionDeclaration func, std::vector<Value> args) override;

        void DebugDumpImpl(Value value, std::string tag, std::ostream& stream) const override;

        Value IntrinsicCall(FunctionDeclaration intrinsic, std::vector<Value> args);

        std::optional<Value> EmitExternalCall(FunctionDeclaration func, std::vector<Value> args);

        bool TypeCompatible(Value value1, Value value2);

        std::string GetScopeAdjustedName(GlobalAllocationScope scope, std::string name) const;
        std::string GetGlobalScopedName(std::string name) const;
        std::string GetCurrentFunctionScopedName(std::string name) const;

        struct PromotedConstantDataDescription
        {
            const ConstantData* data;
            Emittable realValue;
        };

        Value PromoteConstantData(Value value);
        std::optional<PromotedConstantDataDescription> HasBeenPromoted(Value value) const;
        Value Realize(Value value) const;
        Value EnsureEmittable(Value value);

        class IfContextImpl;
        struct FunctionScope;

        std::stack<std::vector<PromotedConstantDataDescription>> _promotedConstantStack;

        emitters::IRModuleEmitter& _emitter;

        // LLVMContext uses ComputeContext internally to handle cases where all relevant operands are constant
        ComputeContext _computeContext;

        std::stack<std::reference_wrapper<emitters::IRFunctionEmitter>> _functionStack;
        std::map<std::string, std::pair<Emittable, MemoryLayout>> _globals;
        std::unordered_map<FunctionDeclaration, DefinedFunction> _definedFunctions;
    };

} // namespace value
} // namespace ell
