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
#include "ValueScalar.h"

#include <functional>
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
        LLVMContext(emitters::IRModuleEmitter&);

    private:
        Value AllocateImpl(ValueType value, MemoryLayout layout) override;

        std::optional<Value> GetGlobalValue(GlobalAllocationScope scope, std::string name) override;

        Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ConstantData data,
                                 MemoryLayout layout) override;
        Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ValueType type,
                                 MemoryLayout layout) override;

        std::pair<ValueType, int> GetTypeImpl(Emittable emittable) override;

        std::function<void()> CreateFunctionImpl(std::string fnName, std::function<void()> fn) override;
        std::function<Value()> CreateFunctionImpl(std::string fnName, Value returnValue, std::function<Value()> fn) override;
        std::function<void(std::vector<Value>)> CreateFunctionImpl(std::string fnName, std::vector<Value> argValues, std::function<void(std::vector<Value>)> fn) override;
        std::function<Value(std::vector<Value>)> CreateFunctionImpl(
            std::string fnName,
            Value returnValue,
            std::vector<Value> argValues,
            std::function<Value(std::vector<Value>)> fn) override;

        Value StoreConstantDataImpl(ConstantData data) override;

        void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn) override;

        void MoveDataImpl(Value& source, Value& destination) override;

        void CopyDataImpl(const Value& source, Value& destination) override;

        Value OffsetImpl(Value begin, Value offset) override;

        Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) override;
        Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) override;

        Value LogicalOperationImpl(ValueLogicalOperation op, Value source1, Value source2) override;

        Value CastImpl(Value value, ValueType type) override;

        IfContext IfImpl(Scalar test, std::function<void()> fn) override;

        Value CallImpl(std::string fnName, Value retValue, std::vector<Value> args) override;

        bool TypeCompatible(Value value1, Value value2);

        std::string GetScopeAdjustedName(GlobalAllocationScope scope, std::string name) const;
        std::string GetGlobalScopedName(std::string name) const;
        std::string GetCurrentFunctionScopedName(std::string name) const;

        emitters::IRFunctionEmitter& GetFnEmitter() const;

        struct PromotedConstantDataDescription
        {
            const ConstantData* data;
            Emittable realValue;
        };

        Value PromoteConstantData(Value value);
        std::optional<PromotedConstantDataDescription> HasBeenPromoted(Value value);
        Value Realize(Value value);
        Value EnsureEmittable(Value value);

        class IfContextImpl;
        struct FunctionScope;

        std::stack<std::vector<PromotedConstantDataDescription>> _promotedConstantStack;

        emitters::IRModuleEmitter& _emitter;

        // LLVMContext uses ComputeContext internally to handle cases where all relevant operands are constant
        ComputeContext _computeContext;

        std::stack<std::reference_wrapper<emitters::IRFunctionEmitter>> _functionStack;
        std::map<std::string, std::pair<Emittable, MemoryLayout>> _globals;
    };

} // namespace value
} // namespace ell