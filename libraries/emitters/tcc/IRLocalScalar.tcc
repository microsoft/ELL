////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValueOperations.tcc (emitters)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    namespace detail
    {
        IREmitter& GetEmitter(IRFunctionEmitter& function);

        template <typename ValueType>
        IRLocalScalar ToIRLocalScalar(IRFunctionEmitter& function, ValueType value)
        {
            return { function, GetEmitter(function).Literal(value) };
        }

        template <typename ValueType, utilities::IsSignedIntegral<ValueType> = true>
        ValueType GetConstantIntValue(llvm::ConstantInt* intValue)
        {
            return static_cast<ValueType>(intValue->getSExtValue());
        }

        template <typename ValueType, utilities::IsUnsignedIntegral<ValueType> = true>
        ValueType GetConstantIntValue(llvm::ConstantInt* intValue)
        {
            return static_cast<ValueType>(intValue->getZExtValue());
        }
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    ValueType IRLocalScalar::GetIntValue() const
    {
        auto intValue = llvm::cast<llvm::ConstantInt>(this->value);
        return detail::GetConstantIntValue<ValueType>(intValue);
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    ValueType IRLocalScalar::GetIntValue(ValueType defaultValue) const
    {
        if (IsConstantInt())
        {
            return GetIntValue<ValueType>();
        }
        
        return defaultValue;
    }

    template <typename ValueType, utilities::IsFloatingPoint<ValueType> /* = true*/>
    ValueType IRLocalScalar::GetFloatValue(ValueType defaultValue) const
    {
        if (IsConstantFloat())
        {
            return GetFloatValue<ValueType>();
        }
        return defaultValue;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator+(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a + b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator+(IRLocalScalar a, ValueType value)
    {
        return value + a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator-(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a - b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator-(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a - b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator*(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a * b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator*(IRLocalScalar a, ValueType value)
    {
        return value * a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator/(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a / b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator/(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a / b;
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    IRLocalScalar operator%(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a % b;
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    IRLocalScalar operator%(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a % b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator==(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a == b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator==(IRLocalScalar a, ValueType value)
    {
        return value == a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator!=(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a != b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator!=(IRLocalScalar a, ValueType value)
    {
        return value != a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a < b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a < b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<=(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a <= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<=(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a <= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a > b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a > b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>=(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a >= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>=(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a >= b;
    }

    //
    // Math functions
    //
    template <typename ValueType>
    IRLocalScalar Sigmoid(IRLocalScalar a)
    {
        auto& fn = a.function;
        auto& emitter = detail::GetEmitter(fn);

        auto expInput = Exp(a);
        constexpr auto one = static_cast<ValueType>(1);
        auto result = emitter.Select(a > ValueType{0}, one / (Exp(-a) + one), expInput / (expInput + one));
        return { fn, result };
    }

    template <typename ValueType>
    IRLocalScalar Tanh(IRLocalScalar a)
    {
        // tanh(x) === (exp(x) - exp(-x)) / (exp(x) + exp(-x))
        //         = 2*sigmoid(2*x) - 1
        auto two = static_cast<ValueType>(2.0);
        return (two * Sigmoid<ValueType>(two * a)) - static_cast<ValueType>(1);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Min(ValueType value, IRLocalScalar b)
    {
        return Min(detail::ToIRLocalScalar(b.function, value), b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Min(IRLocalScalar a, ValueType value)
    {
        return Min(value, a);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Max(ValueType value, IRLocalScalar b)
    {
        return Max(detail::ToIRLocalScalar(b.function, value), b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Max(IRLocalScalar a, ValueType value)
    {
        return Max(value, a);
    }
}
}
