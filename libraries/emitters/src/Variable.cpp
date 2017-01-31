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

    void EmittedVar::Clear()
    {
        isNew = false;
        varIndex = 0;
    }

    //
    // EmittedVariableAllocator
    //
    EmittedVar EmittedVariableAllocator::Allocate()
    {
        EmittedVar var;
        var.isNew = _varStack.IsTopNovel();
        var.varIndex = _varStack.Pop();
        return var;
    }

    void EmittedVariableAllocator::Free(EmittedVar& var)
    {
        _varStack.Push(var.varIndex);
        var.Clear();
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
            case VariableType::Int32:
                return AddVariable<ScalarVariable<int>>(scope);
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
            case VariableType::Int32:
                return AddVariable<VectorVariable<int>>(scope, size);
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
            case VariableType::Int32:
                return AddVariable<VectorElementVariable<int>>(src, offset);
            case VariableType::Byte:
                return AddVariable<VectorElementVariable<uint8_t>>(src, offset);
            default:
                throw EmitterException(EmitterError::valueTypeNotSupported);
        }
    }
}
}
