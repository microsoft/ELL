////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorVariable.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>

namespace ell
{
namespace emitters
{
    template <typename T>
    VectorVariable<T>::VectorVariable(const VariableScope scope, const size_t size, int flags)
        : Variable(GetVariableType<T>(), scope, flags), _size(size)
    {
    }

    template <typename T>
    InitializedVectorVariable<T>::InitializedVectorVariable(const VariableScope scope, const std::vector<T>& data, int flags)
        : VectorVariable<T>(scope, data.size(), flags | Variable::VariableFlags::hasInitValue), _data(std::move(data))
    {
    }

    template <typename T>
    InitializedVectorVariable<T>::InitializedVectorVariable(const VariableScope scope, size_t size, int flags)
        : VectorVariable<T>(scope, size, flags | Variable::VariableFlags::hasInitValue)
    {
        T defValue = GetDefaultValue<T>();
        for (size_t i = 0; i < size; ++i)
        {
            _data.emplace_back(defValue);
        }
    }

    template <typename T>
    LiteralVectorVariable<T>::LiteralVectorVariable(std::vector<T> data)
        : VectorVariable<T>(VariableScope::literal, data.size(), Variable::VariableFlags::none), _data(std::move(data))
    {
    }
}
}
