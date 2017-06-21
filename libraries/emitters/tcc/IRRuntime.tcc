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
    llvm::Function* IRRuntime::GetSqrtFunction()
    {
        return GetSqrtFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    llvm::Function* IRRuntime::GetAbsFunction()
    {
        return GetAbsFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    llvm::Function* IRRuntime::GetExpFunction()
    {
        return GetExpFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    llvm::Function* IRRuntime::GetLogFunction()
    {
        return GetLogFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    llvm::Function* IRRuntime::GetDotProductFunction()
    {
        if (std::is_integral<std::decay_t<ValueType>>::value)
        {
            if (_pDotProductFunction == nullptr)
            {
                _pDotProductFunction = EmitDotProductFunction();
            }
            return _pDotProductFunction;
        }
        else if (std::is_floating_point<std::decay_t<ValueType>>::value)
        {
            if (_pDotProductFunctionFloat == nullptr)
            {
                _pDotProductFunctionFloat = EmitDotProductFunctionF();
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