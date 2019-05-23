////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterContext.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EmitterContext.h"
#include "FunctionDeclaration.h"
#include "Scalar.h"
#include "ScalarOperations.h"
#include "Value.h"
#include "Vector.h"

#include <emitters/include/IREmitter.h>

#include <utilities/include/Exception.h>

#include <iostream>
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
            if (layout == ScalarLayout)
            {
                assert(coordinates.empty());
                return { 0 };
            }
            else
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
        }

    } // namespace detail

    EmitterContext::IfContextImpl::~IfContextImpl() = default;

    EmitterContext::IfContext::IfContext(std::unique_ptr<EmitterContext::IfContextImpl> impl) :
        _impl(std::move(impl))
    {}

    EmitterContext::IfContext&& EmitterContext::IfContext::ElseIf(Scalar test, std::function<void()> fn) &&
    {
        if (test.GetType() != ValueType::Boolean)
        {
            throw InputException(InputExceptionErrors::typeMismatch);
        }

        _impl->ElseIf(test, fn);

        return std::move(*this);
    }

    void EmitterContext::IfContext::Else(std::function<void()> fn) && { _impl->Else(fn); }

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

    detail::ValueTypeDescription EmitterContext::GetType(Emittable emittable) { return GetTypeImpl(emittable); }

    EmitterContext::DefinedFunction EmitterContext::CreateFunction(FunctionDeclaration decl, EmitterContext::DefinedFunction fn)
    {
        return CreateFunctionImpl(decl, fn);
    }

    bool EmitterContext::IsFunctionDefined(FunctionDeclaration decl) const
    {
        return IsFunctionDefinedImpl(decl);
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

    void EmitterContext::For(Scalar start, Scalar stop, Scalar step, std::function<void(Scalar)> fn)
    {
        if (!(start.GetType() == stop.GetType() && start.GetType() == step.GetType()))
        {
            throw InputException(InputExceptionErrors::typeMismatch, "start/stop/step types must match");
        }

        if (start.GetType() == ValueType::Boolean)
        {
            throw InputException(InputExceptionErrors::invalidArgument, "start/stop/step must not be boolean");
        }

        return ForImpl(start, stop, step, fn);
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

    std::optional<Value> EmitterContext::Call(FunctionDeclaration func, std::vector<Value> args)
    {
        return CallImpl(func, args);
    }

    void EmitterContext::DebugDump(Value value, std::string tag, std::ostream* stream) const
    {
        std::ostream& outStream = stream != nullptr ? *stream : std::cerr;

        if (value.IsDefined())
        {
            DebugDumpImpl(value, tag, outStream);
        }
        else
        {
            outStream << "Value is undefined";
            if (!tag.empty())
            {
                outStream << "[tag = " << tag << "]";
            }
            outStream << "\n";
        }
    }

    const std::vector<std::reference_wrapper<FunctionDeclaration>>& EmitterContext::GetIntrinsics() const
    {
        static std::vector intrinsics = {
            std::ref(AbsFunctionDeclaration),
            std::ref(CopySignFunctionDeclaration),
            std::ref(CosFunctionDeclaration),
            std::ref(ExpFunctionDeclaration),
            std::ref(LogFunctionDeclaration),
            std::ref(Log10FunctionDeclaration),
            std::ref(Log2FunctionDeclaration),
            std::ref(MaxNumFunctionDeclaration),
            std::ref(MinNumFunctionDeclaration),
            std::ref(PowFunctionDeclaration),
            std::ref(SinFunctionDeclaration),
            std::ref(SqrtFunctionDeclaration),
            std::ref(TanhFunctionDeclaration),
            std::ref(RoundFunctionDeclaration),
            std::ref(FloorFunctionDeclaration),
            std::ref(CeilFunctionDeclaration)
        };

        return intrinsics;
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

    ContextGuard<>::ContextGuard(EmitterContext& context) :
        _oldContext(s_context)
    {
        SetContext(context);
    }

    ContextGuard<>::~ContextGuard() { _oldContext ? SetContext(*_oldContext) : ClearContext(); }

    Value Allocate(ValueType type, size_t size) { return GetContext().Allocate(type, size); }

    Value Allocate(ValueType type, MemoryLayout layout) { return GetContext().Allocate(type, layout); }

    Value StaticAllocate(std::string name, ValueType type, utilities::MemoryLayout layout)
    {
        return GetContext().StaticAllocate(name, type, layout);
    }

    Value GlobalAllocate(std::string name, ValueType type, utilities::MemoryLayout layout)
    {
        return GetContext().GlobalAllocate(name, type, layout);
    }

    EmitterContext::IfContext If(Scalar test, std::function<void()> fn) { return GetContext().If(test, fn); }

    void ForRange(Scalar end, std::function<void(Scalar)> fn)
    {
        ForRange(0, end, fn);
    }

    void ForRange(Scalar start, Scalar end, std::function<void(Scalar)> fn)
    {
        ForRange(start, end, 1, fn);
    }

    void ForRange(Scalar start, Scalar end, Scalar step, std::function<void(Scalar)> fn)
    {
        GetContext().For(start, end, step, fn);
    }

    void DebugDump(Value value, std::string tag, std::ostream* stream)
    {
        GetContext().DebugDump(value, tag, stream);
    }

    Scalar Abs(Scalar s)
    {
        return *GetContext().Call(AbsFunctionDeclaration, { s.GetValue() });
    }

    Scalar Cos(Scalar s)
    {
        return *GetContext().Call(CosFunctionDeclaration, { s.GetValue() });
    }

    Scalar CopySign(Scalar s1, Scalar s2)
    {
        return *GetContext().Call(CopySignFunctionDeclaration, { s1.GetValue(), s2.GetValue() });
    }

    Scalar Exp(Scalar s)
    {
        return *GetContext().Call(ExpFunctionDeclaration, { s.GetValue() });
    }

    Scalar Log(Scalar s)
    {
        return *GetContext().Call(LogFunctionDeclaration, { s.GetValue() });
    }

    Scalar Log10(Scalar s)
    {
        return *GetContext().Call(Log10FunctionDeclaration, { s.GetValue() });
    }

    Scalar Log2(Scalar s)
    {
        return *GetContext().Call(Log2FunctionDeclaration, { s.GetValue() });
    }

    Scalar Max(Scalar s1, Scalar s2)
    {
        return *GetContext().Call(MaxNumFunctionDeclaration, { s1.GetValue(), s2.GetValue() });
    }

    Scalar Min(Scalar s1, Scalar s2)
    {
        return *GetContext().Call(MinNumFunctionDeclaration, { s1.GetValue(), s2.GetValue() });
    }

    Scalar Pow(Scalar base, Scalar exp)
    {
        return *GetContext().Call(PowFunctionDeclaration, { base.GetValue(), exp.GetValue() });
    }

    Scalar Sin(Scalar s)
    {
        return *GetContext().Call(SinFunctionDeclaration, { s.GetValue() });
    }

    Scalar Sqrt(Scalar s)
    {
        return *GetContext().Call(SqrtFunctionDeclaration, { s.GetValue() });
    }

    Scalar Tanh(Scalar s)
    {
        return *GetContext().Call(TanhFunctionDeclaration, { s.GetValue() });
    }

    Scalar Round(Scalar v)
    {
        return *GetContext().Call(RoundFunctionDeclaration, { v.GetValue() });
    }

    Scalar Floor(Scalar s)
    {
        return *GetContext().Call(FloorFunctionDeclaration, { s.GetValue() });
    }

    Scalar Ceil(Scalar s)
    {
        return *GetContext().Call(CeilFunctionDeclaration, { s.GetValue() });
    }

    Scalar Sign(Scalar s)
    {
        return *GetContext().Call(CopySignFunctionDeclaration, { Cast(1, s.GetType()).GetValue(), s.GetValue() });
    }

    Vector Abs(Vector v)
    {
        return *GetContext().Call(AbsFunctionDeclaration, { v.GetValue() });
    }

    Vector Cos(Vector v)
    {
        return *GetContext().Call(CosFunctionDeclaration, { v.GetValue() });
    }

    Vector Exp(Vector v)
    {
        return *GetContext().Call(ExpFunctionDeclaration, { v.GetValue() });
    }

    Vector Log(Vector v)
    {
        return *GetContext().Call(LogFunctionDeclaration, { v.GetValue() });
    }

    Vector Log10(Vector v)
    {
        return *GetContext().Call(Log10FunctionDeclaration, { v.GetValue() });
    }

    Vector Log2(Vector v)
    {
        return *GetContext().Call(Log2FunctionDeclaration, { v.GetValue() });
    }

    Scalar Max(Vector v)
    {
        return *GetContext().Call(MaxNumFunctionDeclaration, { v.GetValue() });
    }

    Scalar Min(Vector v)
    {
        return *GetContext().Call(MinNumFunctionDeclaration, { v.GetValue() });
    }

    Vector Pow(Vector bases, Scalar exp)
    {
        return *GetContext().Call(PowFunctionDeclaration, { bases.GetValue(), exp.GetValue() });
    }

    Vector Sin(Vector v)
    {
        return *GetContext().Call(SinFunctionDeclaration, { v.GetValue() });
    }

    Vector Sqrt(Vector v)
    {
        return *GetContext().Call(SqrtFunctionDeclaration, { v.GetValue() });
    }

    Scalar Square(const Scalar x)
    {
        auto y = x.Copy();
        return x * y;
    }

    Vector Tanh(Vector v)
    {
        return *GetContext().Call(TanhFunctionDeclaration, { v.GetValue() });
    }

    Scalar LogicalNot(const Scalar v)
    {
        Scalar r;
        if (v.GetType() == ValueType::Boolean)
        {
            ell::utilities::Boolean t(true);
            r = (v != t);
        }
        else
        {
            If(v == Cast(0, v.GetType()), [&] {
                r = Cast(1, v.GetType());
            })
                .Else([&] {
                    r = Cast(0, v.GetType());
                });
        }
        return r;
    }

    Vector Round(Vector v)
    {
        return *GetContext().Call(RoundFunctionDeclaration, { v.GetValue() });
    }
    Vector Floor(Vector v)
    {
        return *GetContext().Call(FloorFunctionDeclaration, { v.GetValue() });
    }

    Vector Ceil(Vector v)
    {
        return *GetContext().Call(CeilFunctionDeclaration, { v.GetValue() });
    }

} // namespace value
} // namespace ell
