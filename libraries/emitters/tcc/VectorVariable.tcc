////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorVariable.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    //
    // VectorVariable
    //
    template <typename T>
    VectorVariable<T>::VectorVariable(const VariableScope scope, const size_t size, int flags)
        : Variable(GetVariableType<T>(), scope, flags), _size(size)
    {
    }

    //
    // InitializedVectorVariable
    //
    template <typename T>
    InitializedVectorVariable<T>::InitializedVectorVariable(const VariableScope scope, const std::vector<T>& data, int flags)
        : VectorVariable<T>(scope, data.size(), flags | Variable::VariableFlags::hasInitValue)
    {
        _initialData = VariableValueType<T>::ToVariableVector(data);
    }

    template <typename T>
    InitializedVectorVariable<T>::InitializedVectorVariable(const VariableScope scope, size_t size, int flags)
        : VectorVariable<T>(scope, size, flags | Variable::VariableFlags::hasInitValue)
    {
        T defValue = GetDefaultValue<ElementType>();
        for (size_t i = 0; i < size; ++i)
        {
            _initialData.emplace_back(defValue);
        }
    }

    //
    // LiteralVectorVariable
    //
    template <typename T>
    LiteralVectorVariable<T>::LiteralVectorVariable(const std::vector<T>& data)
        : VectorVariable<T>(VariableScope::literal, data.size(), Variable::VariableFlags::none)
    {
        _data = VariableValueType<T>::ToVariableVector(data);
    }
}
}
