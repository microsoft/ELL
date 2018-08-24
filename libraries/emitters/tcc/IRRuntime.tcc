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
            if (_pDotProductFunction == nullptr)
            {
                _pDotProductFunction = GetDotProductIntFunction();
            }
            return _pDotProductFunction;
        }
        else if (std::is_floating_point<std::decay_t<ValueType>>::value)
        {
            if (_pDotProductFunctionFloat == nullptr)
            {
                _pDotProductFunctionFloat = GetDotProductFloatFunction();
            }
            return _pDotProductFunctionFloat;
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }
}
}
