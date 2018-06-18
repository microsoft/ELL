////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValue.cpp (emitter)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRLocalValue.h"
#include "IRFunctionEmitter.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>

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

    IRLocalArray::IRLocalArrayValue& IRLocalArray::IRLocalArrayValue::operator=(const IRLocalArrayValue& value)
    {
        // cast the rhs to IRLocalScalar, which decomposes into llvm::Value* thanks to the overload below
        *this = static_cast<IRLocalScalar>(value);

        return *this;
    }

    IRLocalArray::IRLocalArrayValue::operator IRLocalScalar() const
    {
        return _function.LocalScalar(_function.ValueAt(_pPointer, _pOffset));
    }

    IRLocalArray::IRLocalArrayValue& IRLocalArray::IRLocalArrayValue::operator=(llvm::Value* value)
    {
        _function.SetValueAt(_pPointer, _pOffset, value);

        return *this;
    }

    //
    // IRLocalMultidimArray
    //
    IRLocalMultidimArray::IRLocalMultidimArray(emitters::IRFunctionEmitter& function, llvm::Value* data, std::vector<int> extents)
        : IRLocalMultidimArray(function, data, extents, extents)
    {
    }

    IRLocalMultidimArray::IRLocalMultidimArray(emitters::IRFunctionEmitter& function, llvm::Value* data, std::vector<int> extents, std::vector<int> memorySize)
        : function(function), data(data), extents(extents)
    {
        strides.reserve(memorySize.size());
        std::copy(memorySize.begin() + 1, memorySize.end(), std::back_inserter(strides));
        strides.push_back(1);
        int currentStride = 1;
        for (auto it = std::rbegin(strides); it != std::rend(strides); ++it)
        {
            *it *= currentStride;
            currentStride = *it;
        }
    }

    IRLocalMultidimArray::IRLocalArrayElement IRLocalMultidimArray::operator()(std::vector<int> indices) const
    {
        assert(indices.size() == strides.size());
        int offset = 0;
        auto stridesIt = strides.begin();
        for (auto& i : indices)
        {
            offset += i * *stridesIt;
            ++stridesIt;
        }
        return IRLocalArrayElement(function, data, function.Literal(offset));
    }

    IRLocalMultidimArray::IRLocalArrayElement IRLocalMultidimArray::operator()(std::vector<IRLocalScalar> indices) const
    {
        auto offset = function.LocalScalar(0);
        auto stridesIt = strides.begin();
        for (auto& i : indices)
        {
            offset = offset + (i * *stridesIt);
            ++stridesIt;
        }
        return IRLocalArrayElement(function, data, offset);
    }

    IRLocalMultidimArray::IRLocalArrayElement::IRLocalArrayElement(emitters::IRFunctionEmitter& function, llvm::Value* data, llvm::Value* offset)
        : _function(function), _data(data), _offset(offset) {}

    IRLocalMultidimArray::IRLocalArrayElement& IRLocalMultidimArray::IRLocalArrayElement::operator=(const IRLocalArrayElement& value)
    {
        // cast the rhs to IRLocalScalar, which decomposes into llvm::Value* thanks to the overload below
        *this = static_cast<IRLocalScalar>(value);

        return *this;
    }

    IRLocalMultidimArray::IRLocalArrayElement::operator IRLocalScalar() const
    {
        return _function.LocalScalar(_function.ValueAt(_data, _offset));
    }

    IRLocalMultidimArray::IRLocalArrayElement& IRLocalMultidimArray::IRLocalArrayElement::operator=(llvm::Value* value)
    {
        _function.SetValueAt(_data, _offset, value);

        return *this;
    }
}
}
