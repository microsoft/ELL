////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterContext.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EmitterContext.h"
#include "Value.h"
#include "ValueScalar.h"

#include <utilities/include/Exception.h>

#include <tuple>
#include <utility>

namespace ell
{
namespace value
{
    using namespace utilities;

    namespace detail
    {
        Scalar CalculateOffset(const MemoryLayout& layout, std::vector<Scalar> coordinates)
        {
            const auto& offset = layout.GetOffset();
            const auto& increment = layout.GetCumulativeIncrement();
            const auto& order = layout.GetLogicalDimensionOrder();
            const auto numDimensions = layout.NumDimensions();

            Scalar result;
            for (int index = 0; index < numDimensions; ++index)
            {
                result += increment[index] * (coordinates[order[index]] + offset[index]);
            }

            return result;
        }

        std::function<void()> CreateFunction(std::string fnName, std::function<void()> fn)
        {
            auto createdFn = GetContext().CreateFunction(fnName, [fn = std::move(fn)]() -> void { fn(); });

            return [createdFn = std::move(createdFn)]() -> void { createdFn(); };
        }

    } // namespace detail

    EmitterContext::IfContextImpl::~IfContextImpl() = default;

    EmitterContext::IfContext::IfContext(std::unique_ptr<EmitterContext::IfContextImpl> impl) :
        _impl(std::move(impl))
    {}

    EmitterContext::IfContext& EmitterContext::IfContext::ElseIf(Scalar test, std::function<void()> fn)
    {
        if (test.GetType() != ValueType::Boolean)
        {
            throw InputException(InputExceptionErrors::typeMismatch);
        }

        _impl->ElseIf(test, fn);

        return *this;
    }

    void EmitterContext::IfContext::Else(std::function<void()> fn) { _impl->Else(fn); }

    EmitterContext::~EmitterContext() = default;

    Value EmitterContext::Allocate(ValueType type, size_t size) { return Allocate(type, MemoryLayout({ (int)size })); }

    Value EmitterContext::Allocate(ValueType type, MemoryLayout layout) { return AllocateImpl(type, layout); }

    Value EmitterContext::StaticAllocate(std::string name, ValueType type, utilities::MemoryLayout layout)
    {
        if (auto globalValue = GetGlobalValue(GlobalAllocationScope::Function, name))
        {
            Value value = globalValue.value();
            if (layout.GetMemorySize() > value.GetLayout().GetMemorySize())
            {
                throw InputException(InputExceptionErrors::invalidSize);
            }
            value.SetLayout(layout);

            return value;
        }

        return GlobalAllocateImpl(GlobalAllocationScope::Function, name, type, layout);
    }

    Value EmitterContext::GlobalAllocate(std::string name, ValueType type, utilities::MemoryLayout layout)
    {
        if (auto globalValue = GetGlobalValue(GlobalAllocationScope::Global, name))
        {
            Value value = globalValue.value();
            if (layout.GetMemorySize() > value.GetLayout().GetMemorySize())
            {
                throw InputException(InputExceptionErrors::invalidSize);
            }
            value.SetLayout(layout);

            return value;
        }

        return GlobalAllocateImpl(GlobalAllocationScope::Global, name, type, layout);
    }

    std::pair<ValueType, int> EmitterContext::GetType(Emittable emittable) { return GetTypeImpl(emittable); }

    std::function<void()> EmitterContext::CreateFunction(std::string fnName, std::function<void()> fn)
    {
        return CreateFunctionImpl(fnName, fn);
    }

    std::function<void(std::vector<Value>)> EmitterContext::CreateFunction(std::string fnName, std::vector<Value> argTypes, std::function<void(std::vector<Value>)> fn)
    {
        return CreateFunctionImpl(fnName, argTypes, fn);
    }

    std::function<Value()> EmitterContext::CreateFunction(std::string fnName, Value returnValue, std::function<Value()> fn)
    {
        return CreateFunctionImpl(fnName, returnValue, fn);
    }

    std::function<Value(std::vector<Value>)> EmitterContext::CreateFunction(std::string fnName, Value returnValue, std::vector<Value> argTypes, std::function<Value(std::vector<Value>)> fn)
    {
        return CreateFunctionImpl(fnName, returnValue, argTypes, fn);
    }

    Value EmitterContext::StoreConstantData(ConstantData data) { return StoreConstantDataImpl(data); }

    void EmitterContext::For(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn)
    {
        if (layout.NumElements() == 0)
        {
            return;
        }

        return ForImpl(layout, fn);
    }

    void EmitterContext::MoveData(Value& source, Value& destination) { return MoveDataImpl(source, destination); }

    void EmitterContext::CopyData(const Value& source, Value& destination) { return CopyDataImpl(source, destination); }

    Value EmitterContext::Offset(Value begin, Value index) { return OffsetImpl(begin, index); }

    Value EmitterContext::Offset(Value begin, std::vector<Scalar> coordinates)
    {
        const auto& layout = begin.GetLayout();

        Scalar result = detail::CalculateOffset(layout, coordinates);
        return Offset(begin, result.GetValue());
    }

    Value EmitterContext::UnaryOperation(ValueUnaryOperation op, Value value) { return UnaryOperationImpl(op, value); }

    Value EmitterContext::BinaryOperation(ValueBinaryOperation op, Value destination, Value source)
    {
        return BinaryOperationImpl(op, destination, source);
    }

    Value EmitterContext::LogicalOperation(ValueLogicalOperation op, Value source1, Value source2)
    {
        if (!source1.IsDefined() || !source2.IsDefined())
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }

        if (source1.GetBaseType() != source2.GetBaseType())
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }

        return LogicalOperationImpl(op, source1, source2);
    }

    Value EmitterContext::Cast(Value value, ValueType type)
    {
        if (!value.IsConstrained())
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }

        return CastImpl(value, type);
    }

    EmitterContext::IfContext EmitterContext::If(Scalar test, std::function<void()> fn)
    {
        if (test.GetType() != ValueType::Boolean)
        {
            throw InputException(InputExceptionErrors::typeMismatch);
        }

        return IfImpl(test, fn);
    }

    Value EmitterContext::Call(std::string fnName, Value retValue, std::vector<Value> args)
    {
        return CallImpl(fnName, retValue, args);
    }

    namespace
    {
        EmitterContext* s_context = nullptr;
    }

    EmitterContext& GetContext()
    {
        if (s_context == nullptr)
        {
            throw LogicException(LogicExceptionErrors::illegalState, "EmitterContext is not set!");
        }

        return *s_context;
    }

    void SetContext(EmitterContext& context) { s_context = &context; }

    void ClearContext() noexcept { s_context = nullptr; }

    ContextGuard::ContextGuard(EmitterContext& context) { SetContext(context); }

    ContextGuard::~ContextGuard() { ClearContext(); }

    std::function<void()> CreateFunction(std::string fnName, std::function<void()> fn)
    {
        return GetContext().CreateFunction(fnName, fn);
    }

    std::function<Value()> CreateFunction(std::string fnName, Value returnValue, std::function<Value()> fn)
    {
        return GetContext().CreateFunction(fnName, returnValue, fn);
    }

    std::function<Value(std::vector<Value>)> CreateFunction(std::string fnName, Value returnValue, std::vector<Value> argTypes, std::function<Value(std::vector<Value>)> fn)
    {
        return GetContext().CreateFunction(fnName, returnValue, argTypes, fn);
    }

    std::function<Value(std::vector<Value>)> CreateFunction(std::string fnName, Value returnValue, std::initializer_list<Value> argTypes, std::function<Value(std::vector<Value>)> fn)
    {
        return CreateFunction(fnName, returnValue, std::vector<Value>(argTypes), fn);
    }

    Value Allocate(ValueType type, size_t size) { return GetContext().Allocate(type, size); }

    Value Allocate(ValueType type, MemoryLayout layout) { return GetContext().Allocate(type, layout); }

    Value GlobalAllocate(std::string name, ValueType type, utilities::MemoryLayout layout)
    {
        return GetContext().GlobalAllocate(name, type, layout);
    }

    EmitterContext::IfContext If(Scalar test, std::function<void()> fn) { return GetContext().If(test, fn); }

    Value Call(std::string fnName, Value retValue, std::vector<Value> args)
    {
        return GetContext().Call(fnName, retValue, args);
    }

} // namespace value
} // namespace ell
