////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMContext.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterContext.h"
#include "ComputeContext.h"
#include "ValueScalar.h"

// stl
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

        std::pair<ValueType, int> GetTypeImpl(Emittable emittable) override;

        std::function<void()> CreateFunctionImpl(std::string fnName, std::function<void()> fn) override;
        std::function<Value()> CreateFunctionImpl(std::string fnName, Value returnValue,
                                                  std::function<Value()> fn) override;
        std::function<void(std::vector<Value>)> CreateFunctionImpl(std::string fnName, std::vector<Value> argValues,
                                                                   std::function<void(std::vector<Value>)> fn) override;
        std::function<Value(std::vector<Value>)> CreateFunctionImpl(
            std::string fnName, Value returnValue, std::vector<Value> argValues,
            std::function<Value(std::vector<Value>)> fn) override;

        Value StoreConstantDataImpl(ConstantData data) override;

        void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn) override;

        void MoveDataImpl(Value& source, Value& destination) override;

        void CopyDataImpl(const Value& source, Value& destination) override;

        Value OffsetImpl(Value begin, Value offset) override;

        Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) override;
        Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) override;

        bool TypeCompatible(Value value1, Value value2);

        struct FunctionScope;

        emitters::IRModuleEmitter& _emitter;

        // LLVMContext uses ComputeContext internally to handle cases where all relevant operands are constant
        ComputeContext _computeContext;

        std::stack<std::reference_wrapper<emitters::IRFunctionEmitter>> _functionStack;
    };

} // namespace value
} // namespace ell