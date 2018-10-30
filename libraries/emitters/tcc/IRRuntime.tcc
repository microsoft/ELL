////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRRuntime.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename ValueType>
    LLVMFunction IRRuntime::GetSqrtFunction()
    {
        return GetSqrtFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetAbsFunction()
    {
        return GetAbsFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetExpFunction()
    {
        return GetExpFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetLogFunction()
    {
        return GetLogFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetTanhFunction()
    {
        return GetTanhFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetSinFunction()
    {
        return GetSinFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetCosFunction()
    {
        return GetCosFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetDotProductFunction()
    {
        if (std::is_integral<std::decay_t<ValueType>>::value)
        {
            if (_dotProductFunction == nullptr)
            {
                _dotProductFunction = GetDotProductIntFunction();
            }
            return _dotProductFunction;
        }
        else if (std::is_floating_point<std::decay_t<ValueType>>::value)
        {
            if (_dotProductFunctionFloat == nullptr)
            {
                _dotProductFunctionFloat = GetDotProductFloatFunction();
            }
            return _dotProductFunctionFloat;
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }
}
}
