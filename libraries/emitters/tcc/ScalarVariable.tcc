////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalarVariable.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename T>
    ScalarVariable<T>::ScalarVariable(const VariableScope scope, int flags)
        : Variable(GetVariableType<T>(), scope, flags)
    {
    }

    template <typename T>
    LiteralVariable<T>::LiteralVariable(T data)
        : ScalarVariable<T>(VariableScope::literal, Variable::VariableFlags::none), _data(data)
    {
    }

    template <typename T>
    InitializedScalarVariable<T>::InitializedScalarVariable(const VariableScope scope, T data, bool isMutable)
        : ScalarVariable<T>(scope, isMutable ? (Variable::VariableFlags::isMutable | Variable::VariableFlags::hasInitValue) : Variable::VariableFlags::hasInitValue), _data(data)
    {
    }

    template <typename T>
    VectorElementVariable<T>::VectorElementVariable(Variable& src, int offset)
        : ScalarVariable<T>(VariableScope::local, Variable::VariableFlags::isVectorRef), _src(src), _offset(offset)
    {
    }
}
}