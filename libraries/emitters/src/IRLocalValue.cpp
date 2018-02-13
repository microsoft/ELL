////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValue.cpp (emitter)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRLocalValue.h"
#include "IRFunctionEmitter.h"

namespace ell
{
namespace emitters
{
    //
    // Utility code
    //
    namespace
    {
        void VerifyFromSameFunction(const IRLocalValue& a, const IRLocalValue& b)
        {
            if (a.function.GetFunction() != b.function.GetFunction())
            {
                throw EmitterException(EmitterError::badFunctionArguments, "IRLocalValue arguments are local to different functions");
            }
        }
    }

    //
    // IRLocalValue
    //
    IRLocalValue::IRLocalValue(emitters::IRFunctionEmitter& function, llvm::Value* value)
        : function(function), value(value) {}

    IRLocalValue& IRLocalValue::operator=(const IRLocalValue& other)
    {
        VerifyFromSameFunction(*this, other);
        this->value = other.value;
        return *this;
    }

    IRLocalValue& IRLocalValue::operator=(llvm::Value* value)
    {
        this->value = value;
        return *this;
    }

    //
    // IRLocalArrayValue
    //
    IRLocalArray::IRLocalArrayValue IRLocalArray::operator[](int offset) const
    {
        return IRLocalArrayValue(function, value, function.Literal(offset));
    }

    IRLocalArray::IRLocalArrayValue IRLocalArray::operator[](llvm::Value* offset) const
    {
        return IRLocalArrayValue(function, value, offset);
    }

    IRLocalArray::IRLocalArrayValue::IRLocalArrayValue(
        emitters::IRFunctionEmitter& function, llvm::Value* value, llvm::Value* pOffset)
        : _function(function), _pPointer(value), _pOffset(pOffset) {}

    IRLocalArray::IRLocalArrayValue::operator IRLocalScalar() const
    {
        return _function.LocalScalar(_function.ValueAt(_pPointer, _pOffset));
    }

    IRLocalArray::IRLocalArrayValue& IRLocalArray::IRLocalArrayValue::operator=(llvm::Value* value)
    {
        _function.SetValueAt(_pPointer, _pOffset, value);

        return *this;
    }
}
}
