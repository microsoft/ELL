////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalarVariable.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Variable.h"

namespace ell
{
namespace emitters
{
    ///<summary>A scalar variable</summary>
    template <typename T>
    class ScalarVariable : public Variable
    {
    public:
        ///<summary>Construct a new scalar variable</summary>
        ScalarVariable(const VariableScope scope, int flags = VariableFlags::isMutable);

        ///<summary>Returns true</summary>
        bool IsScalar() const override { return true; }
    };

    ///<summary>A literal scalar</summary>
    template <typename T>
    class LiteralVariable : public ScalarVariable<T>
    {
    public:
        ///<summary>Construct a new literal</summary>
        LiteralVariable(T data);

        ///<summary>The literal value</summary>
        const T& Data() const { return _data; }

    private:
        T _data;
    };

    ///<summary>A scalar variable with an initial value</summary>
    template <typename T>
    class InitializedScalarVariable : public ScalarVariable<T>
    {
    public:
        ///<summary>Construct a scalar that will have the given initial value</summary>
        InitializedScalarVariable(const VariableScope scope, T data, bool isMutable = true);

        ///<summary>Initial value for this scalar</summary>
        const T& Data() const { return _data; }

    private:
        T _data;
    };

    ///<summary>A scalar variable that is a reference into a vector variable</summary>
    template <typename T>
    class VectorElementVariable : public ScalarVariable<T>
    {
    public:
        ///<summary>Construct a vector element</summary>
        VectorElementVariable(Variable& src, int offset);

        ///<summary>The source vector this is an offset into</summary>
        Variable& Src() const { return _src; }

        ///<summary>Offset into vector</summary>
        int Offset() const { return _offset; }

    private:
        Variable& _src;
        int _offset;
    };
}
}

#include "../tcc/ScalarVariable.tcc"