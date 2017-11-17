////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variable.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{

    template <typename T>
    class ScalarVariable;

    template <typename T>
    class InitializedScalarVariable;

    template <typename T>
    class VectorVariable;

    template <typename T>
    class InitializedVectorVariable;

    //
    // VariableAllocator
    //
    template <typename VarType, typename... Args>
    VarType* VariableAllocator::AddVariable(Args&&... args)
    {
        static_assert(std::is_base_of<Variable, VarType>::value, "AddVariable requires you inherit from Variable");

        auto var = std::make_shared<VarType>(args...);
        VarType* pVar = var.get();
        _variables.push_back(var);
        return pVar;
    }

    template <typename ElementType>
    Variable* VariableAllocator::AddScalarVariable(VariableScope scope)
    {
        return AddVariable<ScalarVariable<ElementType>>(scope);
    }

    template <typename ElementType>
    Variable* VariableAllocator::AddScalarVariable(VariableScope scope, ElementType value)
    {
        return AddVariable<InitializedScalarVariable<ElementType>>(scope, value);
    }

    template <typename ElementType>
    Variable* VariableAllocator::AddVectorVariable(VariableScope scope, int size)
    {
        return AddVariable<VectorVariable<ElementType>>(scope, size);
    }

    /// <summary> Add a vector, with all elements initialized to a given value </summary>
    template <typename ElementType>
    Variable* VariableAllocator::AddVectorVariable(VariableScope scope, int size, ElementType value)
    {
        std::vector<ElementType> data(size, value);
        return AddVariable<InitializedVectorVariable<ElementType>>(scope, data);
    }

    /// <summary> Add a vector, initialized to a given vector </summary>
    template <typename ElementType>
    Variable* VariableAllocator::AddVectorVariable(VariableScope scope, const std::vector<ElementType>& data)
    {
        return AddVariable<InitializedVectorVariable<ElementType>>(scope, data);
    }
}
}
