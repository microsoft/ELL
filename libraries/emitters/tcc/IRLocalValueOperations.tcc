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
    namespace impl
    {
        template <typename ValueType, utilities::IsFundamental<ValueType> = true>
        void VerifyArgTypesCompatible(const IRLocalValue& a, ValueType b)
        {
            if ((a.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{}) ||
                (a.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{}))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "IRLocalValue arguments have incompatible types");
            }
        }

        template <typename ValueType, utilities::IsFundamental<ValueType> = true>
        void VerifyArgTypesCompatible(ValueType a, const IRLocalValue& b)
        {
            VerifyArgTypesCompatible(b, a);
        }
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator+(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) + b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator+(IRLocalScalar a, ValueType b)
    {
        return b + a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator-(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) - b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator-(IRLocalScalar a, ValueType b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return a - a.function.LocalScalar(b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator*(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) * b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator*(IRLocalScalar a, ValueType b)
    {
        return b * a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator/(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) / b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator/(IRLocalScalar a, ValueType b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return a / a.function.LocalScalar(b);
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    IRLocalScalar operator%(ValueType a, IRLocalScalar b)
    {
        if (b.value->getType()->isIntegerTy())
        {
            return b.function.LocalScalar(a) % b;
        }
        throw EmitterException(EmitterError::badFunctionArguments, "IRLocalValue arguments must be integral");
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    IRLocalScalar operator%(IRLocalScalar a, ValueType b)
    {
        if (a.value->getType()->isIntegerTy())
        {
            return a % a.function.LocalScalar(b);
        }
        throw EmitterException(EmitterError::badFunctionArguments, "IRLocalValue arguments must be integral");
    }
    
    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator==(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) == b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator==(IRLocalScalar a, ValueType b)
    {
        return b == a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator!=(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) != b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator!=(IRLocalScalar a, ValueType b)
    {
        return b != a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) < b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<(IRLocalScalar a, ValueType b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return a < a.function.LocalScalar(b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<=(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) <= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<=(IRLocalScalar a, ValueType b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return a <= a.function.LocalScalar(b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) > b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>(IRLocalScalar a, ValueType b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return a > a.function.LocalScalar(b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>=(ValueType a, IRLocalScalar b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return b.function.LocalScalar(a) >= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>=(IRLocalScalar a, ValueType b)
    {
        impl::VerifyArgTypesCompatible(a, b);
        return a >= a.function.LocalScalar(b);
    }

    template <typename ValueType>
    IRLocalScalar Sigmoid(IRLocalScalar a)
    {
        auto& fn = a.function;

        auto expInput = Exp(a);
        constexpr auto one = static_cast<ValueType>(1);
        auto result = fn.Select(a > ValueType{0}, one / (Exp(-a) + one), expInput / (expInput + one));
        return fn.LocalScalar(result);
    }

    template <typename ValueType>
    IRLocalScalar Tanh(IRLocalScalar a)
    {
        // tanh(x) === (exp(x) - exp(-x)) / (exp(x) + exp(-x))
        //         = 2*sigmoid(2*x) - 1
        auto two = static_cast<ValueType>(2.0);
        return (two * Sigmoid<ValueType>(two * a)) - static_cast<ValueType>(1);
    }
}
}

