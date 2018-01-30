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
    IRLocalValue::IRLocalValue(emitters::IRFunctionEmitter& function, llvm::Value* value)
        : function(function), value(value) {}

    IRLocalArray::IRLocalArray(emitters::IRFunctionEmitter& function, llvm::Value* pPointer)
        : function(function), pPointer(pPointer) {}

    IRLocalArray::IRLocalArrayValue IRLocalArray::operator[](int offset) const
    {
        return IRLocalArrayValue(function, pPointer, function.Literal(offset));
    }

    IRLocalArray::IRLocalArrayValue IRLocalArray::operator[](llvm::Value* offset) const
    {
        return IRLocalArrayValue(function, pPointer, offset);
    }

    IRLocalArray::IRLocalArrayValue::IRLocalArrayValue(
        emitters::IRFunctionEmitter& function, llvm::Value* pPointer, llvm::Value* pOffset)
        : _function(function), _pPointer(pPointer), _pOffset(pOffset) {}

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
