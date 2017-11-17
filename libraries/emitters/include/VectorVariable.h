////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorVariable.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterTypes.h"
#include "Variable.h"

// stl
#include <vector>

namespace ell
{
namespace emitters
{
    /// <summary> A variable that represents a vector (array) </summary>
    template <typename T>
    class VectorVariable : public Variable
    {
    public:
        /// <summary> Create a new vector variable </summary>
        VectorVariable(const VariableScope scope, const size_t size, int flags = Variable::VariableFlags::isMutable);

        /// <summary> The size of the vector </summary>
        size_t Dimension() const override { return _size; }

        /// <summary> Is this variable a scalar? Returns false. </summary>
        bool IsScalar() const override { return false; }

    private:
        int _size;
    };

    /// <summary> A vector variable that is intialized with some the given vector data </summary>
    template <typename T>
    class InitializedVectorVariable : public VectorVariable<T>
    {
    public:
        using ElementType = typename VariableValueType<T>::type;

        /// <summary> Create a new vector variable initialized with the given data </summary>
        InitializedVectorVariable(const VariableScope scope, const std::vector<T>& data, int flags = Variable::VariableFlags::isMutable);

        /// <summary> Create a new vector variable of the given size, initialized to the default value for the template type </summary>
        InitializedVectorVariable(const VariableScope scope, size_t size, int flags = Variable::VariableFlags::isMutable);

        /// <summary> The data this vector is initialized with </summary>
        const std::vector<T> Data() const
        {
            return VariableValueType<T>::FromVariableVector(_initialData);
        }

    private:
        std::vector<ElementType> _initialData;
    };

    /// <summary>
    /// A vector variable that is typically emitted as a static const or global - depending on the language
    /// </summary>
    template <typename T>
    class LiteralVectorVariable : public VectorVariable<T>
    {
    public:
        using ElementType = typename VariableValueType<T>::type;

        /// <summary> Create a new literal using the given data </summary>
        LiteralVectorVariable(const std::vector<T>& data);

        /// <summary> The data this vector is initialized with </summary>
        std::vector<T> Data() const { return VariableValueType<T>::FromVariableVector(_data); }

    private:
        std::vector<ElementType> _data;
    };
}
}

#include "../tcc/VectorVariable.tcc"