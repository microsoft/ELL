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
    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator+(ValueType a, IRLocalScalar b)
    {
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) + b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) + b;
        }

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
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) - b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) - b;
        }

        return b.function.LocalScalar(a) - b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator-(IRLocalScalar a, ValueType b)
    {
        if (a.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return a - a.function.LocalScalar(static_cast<double>(b));
        }
        else if (a.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return a - a.function.LocalScalar(static_cast<long>(b));
        }

        return a - a.function.LocalScalar(b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator*(ValueType a, IRLocalScalar b)
    {
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) * b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) * b;
        }

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
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) / b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) / b;
        }

        return b.function.LocalScalar(a) / b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator/(IRLocalScalar a, ValueType b)
    {
        if (a.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return a / a.function.LocalScalar(static_cast<double>(b));
        }
        else if (a.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return a / a.function.LocalScalar(static_cast<long>(b));
        }

        return a / a.function.LocalScalar(b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator==(ValueType a, IRLocalScalar b)
    {
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) == b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) == b;
        }

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
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) != b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) != b;
        }

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
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) < b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) < b;
        }

        return b.function.LocalScalar(a) < b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<(IRLocalScalar a, ValueType b)
    {
        return b >= a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<=(ValueType a, IRLocalScalar b)
    {
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) <= b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) <= b;
        }

        return b.function.LocalScalar(a) <= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<=(IRLocalScalar a, ValueType b)
    {
        return b > a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>(ValueType a, IRLocalScalar b)
    {
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) > b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) > b;
        }

        return b.function.LocalScalar(a) > b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>(IRLocalScalar a, ValueType b)
    {
        return b <= a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>=(ValueType a, IRLocalScalar b)
    {
        if (b.value->getType()->isFloatingPointTy() && !std::is_floating_point<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<double>(a)) >= b;
        }
        else if (b.value->getType()->isIntegerTy() && !std::is_integral<ValueType>{})
        {
            return b.function.LocalScalar(static_cast<long>(a)) >= b;
        }

        return b.function.LocalScalar(a) >= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>=(IRLocalScalar a, ValueType b)
    {
        return b < a;
    }
}
}

