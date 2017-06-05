////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variable.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Variable.h"
#include "EmitterException.h"
#include "ScalarVariable.h"
#include "VectorVariable.h"

namespace ell
{
namespace emitters
{
    //
    // EmittedVariable
    //
    void EmittedVariable::Clear()
    {
        isNew = false;
        varIndex = 0;
    }

    //
    // EmittedVariableAllocator
    //
    EmittedVariable EmittedVariableAllocator::Allocate()
    {
        EmittedVariable var;
        var.isNew = _varStack.IsTopNovel();
        var.varIndex = _varStack.Pop();
        return var;
    }

    void EmittedVariableAllocator::Free(EmittedVariable& var)
    {
        _varStack.Push(var.varIndex);
        var.Clear();
    }

    //
    // Variable
    //
    Variable::Variable(const VariableType type, const VariableScope scope, int flags)
        : _type(type), _scope(scope), _flags(flags)
    {
    }

    void Variable::SetEmittedName(std::string name)
    {
        _emittedName = std::move(name);
    }

    bool Variable::TestFlags(int flags) const
    {
        return (_flags & flags) != 0;
    }

    void Variable::AssignVariable(EmittedVariable variable)
    {
        _emittedVar = variable;
    }

    void Variable::SetFlags(const VariableFlags flag)
    {
        _flags |= (int)flag;
    }

    void Variable::ClearFlags(const VariableFlags flag)
    {
        _flags &= (~((int)flag));
    }
    //
    // VariableAllocator
    //
    Variable* VariableAllocator::AddScalarVariable(VariableScope scope, VariableType type)
    {
        switch (type)
        {
            case VariableType::Double:
                return AddVariable<ScalarVariable<double>>(scope);
            case VariableType::Float:
                return AddVariable<ScalarVariable<float>>(scope);
            case VariableType::Int32:
                return AddVariable<ScalarVariable<int>>(scope);
            case VariableType::Int64:
                return AddVariable<ScalarVariable<int64_t>>(scope);
            case VariableType::Byte:
                return AddVariable<ScalarVariable<uint8_t>>(scope);
            default:
                throw EmitterException(EmitterError::valueTypeNotSupported);
        }
    }

    Variable* VariableAllocator::AddVectorVariable(VariableScope scope, VariableType type, int size)
    {
        switch (type)
        {
            case VariableType::Double:
                return AddVariable<VectorVariable<double>>(scope, size);
            case VariableType::Float:
                return AddVariable<VectorVariable<float>>(scope, size);
            case VariableType::Int32:
                return AddVariable<VectorVariable<int>>(scope, size);
            case VariableType::Int64:
                return AddVariable<VectorVariable<int64_t>>(scope, size);
            case VariableType::Byte:
                return AddVariable<VectorVariable<uint8_t>>(scope, size);
            default:
                throw EmitterException(EmitterError::valueTypeNotSupported);
        }
    }

    Variable* VariableAllocator::AddVectorElementVariable(VariableType type, Variable& src, int offset)
    {
        switch (type)
        {
            case VariableType::Double:
                return AddVariable<VectorElementVariable<double>>(src, offset);
            case VariableType::Float:
                return AddVariable<VectorElementVariable<float>>(src, offset);
            case VariableType::Int32:
                return AddVariable<VectorElementVariable<int>>(src, offset);
            case VariableType::Int64:
                return AddVariable<VectorElementVariable<int64_t>>(src, offset);
            case VariableType::Byte:
                return AddVariable<VectorElementVariable<uint8_t>>(src, offset);
            default:
                throw EmitterException(EmitterError::valueTypeNotSupported);
        }
    }
}
}
