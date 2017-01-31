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
}
}