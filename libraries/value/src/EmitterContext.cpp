////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterContext.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EmitterContext.h"
#include "Scalar.h"
#include "Value.h"

// utilities
#include "Exception.h"

// stl
#include <tuple>
#include <utility>

namespace ell
{
namespace value
{
using namespace utilities;

namespace detail
{

std::function<void()> CreateFunction(std::string fnName, std::function<void()> fn)
    {
    auto createdFn = GetContext().CreateFunction(fnName, [fn = std::move(fn)]() -> void
    {
        fn();
    });

    return [createdFn = std::move(createdFn)]() -> void
    {
        createdFn();
    };
}

}

EmitterContext::~EmitterContext() = default;

Value EmitterContext::Allocate(ValueType type, size_t size)
{
    return Allocate(type, MemoryLayout({ (int)size }));
}

Value EmitterContext::Allocate(ValueType type, MemoryLayout layout)
{
    return AllocateImpl(type, layout);
}

// Value EmitterContext::GlobalAllocate(ValueType, size_t)
// Value EmitterContext::GlobalAllocate(ValueType, MemoryLayout)

std::pair<ValueType, int> EmitterContext::GetType(Emittable emittable)
{
    return GetTypeImpl(emittable);
}

std::function<void()> EmitterContext::CreateFunction(std::string fnName, std::function<void()> fn)
{
    return CreateFunctionImpl(fnName, fn);
}

std::function<Value()> EmitterContext::CreateFunction(std::string fnName, Value returnValue, std::function<Value()> fn)
{
    if (!returnValue.IsEmpty()) { throw InputException(InputExceptionErrors::invalidArgument); }

    return CreateFunctionImpl(fnName, returnValue, fn);
}

std::function<Value(std::vector<Value>)> EmitterContext::CreateFunction(
    std::string fnName, Value returnValue, std::vector<Value> argTypes, std::function<Value(std::vector<Value>)> fn)
{
    if (std::any_of(argTypes.begin(), argTypes.end(), [](auto value){ return !value.IsEmpty(); }) ||
        !returnValue.IsEmpty()
    )
    {
        throw InputException(InputExceptionErrors::invalidArgument);
    }

    return CreateFunctionImpl(fnName, returnValue, argTypes, fn);
}

Value EmitterContext::StoreConstantData(ConstantData data)
{
    return StoreConstantDataImpl(data);
}

void EmitterContext::For(Value value, MemoryLayout layout, std::function<void(MemoryCoordinates)> fn)
{
    return ForImpl(value, layout, fn);
}

void EmitterContext::MoveData(Value& source, Value& destination)
{
    return MoveDataImpl(source, destination);
}

void EmitterContext::CopyData(const Value& source, Value& destination)
{
    return CopyDataImpl(source, destination);
}

Value EmitterContext::Offset(Value begin, Value index)
{
    return OffsetImpl(begin, index);
}

Value EmitterContext::Offset(Value begin, std::vector<Scalar> coordinates)
{
    const auto& layout = begin.GetLayout();
    const auto& offset = layout.GetOffset();
    const auto& increment = layout.GetCumulativeIncrement();
    const auto& order = layout.GetLogicalDimensionOrder();
    const auto numDimensions = layout.NumDimensions();

    Scalar result;
    for (int index = 0; index < numDimensions; ++index)
    {
        result += increment[index] * (coordinates[order[index]] + offset[index]);
    }
    return Offset(begin, static_cast<Value>(result));
}

Value EmitterContext::UnaryOperation(ValueUnaryOperation op, Value value)
{
    return UnaryOperationImpl(op, value);
}

Value EmitterContext::BinaryOperation(ValueBinaryOperation op, Value destination, Value source)
{
    return BinaryOperationImpl(op, destination, source);
}

namespace {
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

void SetContext(EmitterContext& context)
{
    s_context = &context;
}

void ClearContext() noexcept
{
    s_context = nullptr;
}

ContextGuard::ContextGuard(EmitterContext& context)
{
    SetContext(context);
}

ContextGuard::~ContextGuard() { ClearContext(); }

std::function<void()> CreateFunction(std::string fnName, std::function<void()> fn)
{
    return GetContext().CreateFunction(fnName, fn);
}

std::function<Value()> CreateFunction(std::string fnName, Value returnValue, std::function<Value()> fn)
{
    return GetContext().CreateFunction(fnName, returnValue, fn);
}

std::function<Value(std::vector<Value>)> CreateFunction(
    std::string fnName, Value returnValue, std::vector<Value> argTypes,
    std::function<Value(std::vector<Value>)> fn)
{
    return GetContext().CreateFunction(fnName, returnValue, argTypes, fn);
}

std::function<Value(std::vector<Value>)> CreateFunction(
    std::string fnName, Value returnValue, std::initializer_list<Value> argTypes,
    std::function<Value(std::vector<Value>)> fn)
{
    return CreateFunction(fnName, returnValue, std::vector<Value>(argTypes), fn);
}

Value Allocate(ValueType type, size_t size)
{
    return GetContext().Allocate(type, size);
}

Value Allocate(ValueType type, MemoryLayout layout)
{
    return GetContext().Allocate(type, layout);
}

} // value
} // ell