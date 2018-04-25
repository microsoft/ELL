////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValue.cpp (emitter)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRLocalValue.h"
#include "IRFunctionEmitter.h"

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
    // IRLocalMatrix
    //
    IRLocalMatrix::IRLocalMatrix(emitters::IRFunctionEmitter& function, llvm::Value* data, int rows, int columns)
        : function(function), data(data), rows(rows), columns(columns) {}

    IRLocalMatrix::IRLocalMatrixValue IRLocalMatrix::operator()(int row, int column) const
    {
        return IRLocalMatrixValue(function, data, function.Literal(row*columns + column));
    }

    IRLocalMatrix::IRLocalMatrixValue IRLocalMatrix::operator()(llvm::Value* row, llvm::Value* column) const
    {
        auto r = function.LocalScalar(row);
        auto c = function.LocalScalar(column);
        return IRLocalMatrixValue(function, data, (r*columns + c));
    }

    IRLocalMatrix::IRLocalMatrixValue::IRLocalMatrixValue(emitters::IRFunctionEmitter& function, llvm::Value* data, llvm::Value* offset)
        : _function(function), _data(data), _offset(offset) {}

    IRLocalMatrix::IRLocalMatrixValue::operator IRLocalScalar() const
    {
        return _function.LocalScalar(_function.ValueAt(_data, _offset));
    }

    IRLocalMatrix::IRLocalMatrixValue& IRLocalMatrix::IRLocalMatrixValue::operator=(llvm::Value* value)
    {
        _function.SetValueAt(_data, _offset, value);

        return *this;
    }

    //
    // IRLocalMultidimArray
    //
    IRLocalMultidimArray::IRLocalMultidimArray(emitters::IRFunctionEmitter& function, llvm::Value* data, std::initializer_list<int> dimensions) 
        : IRLocalMultidimArray(function, data, dimensions, dimensions)
        {
        }

    IRLocalMultidimArray::IRLocalMultidimArray(emitters::IRFunctionEmitter& function, llvm::Value* data, std::initializer_list<int> dimensions, std::initializer_list<int> memorySize) 
        : function(function), data(data), dimensions(dimensions.begin(), dimensions.end())
        {
            strides.reserve(memorySize.size());
            std::copy(memorySize.begin()+1, memorySize.end(), std::back_inserter(strides)); 
            strides.push_back(1);
            int currentStride = 1;
            for(auto it = std::rbegin(strides); it != std::rend(strides); ++it)
            {
                *it *= currentStride;
                currentStride = *it;
            }
        }

    IRLocalMultidimArray::IRLocalArrayElement IRLocalMultidimArray::operator()(std::initializer_list<int> indices) const
    {
        assert(indices.size() == strides.size());
        int offset = 0;
        auto stridesIt = strides.begin();
        for(auto& i: indices)
        {
            offset += i * *stridesIt;
            ++stridesIt;
        }
        return IRLocalArrayElement(function, data, function.Literal(offset));
    }

    IRLocalMultidimArray::IRLocalArrayElement IRLocalMultidimArray::operator()(std::initializer_list<llvm::Value*> indices) const
    {
        auto offset = function.LocalScalar(0);
        auto stridesIt = strides.begin();
        for(auto& i: indices)
        {
            offset = offset + function.LocalScalar(i) * *stridesIt;
            ++stridesIt;
        }
        return IRLocalArrayElement(function, data, offset);
    }

    IRLocalMultidimArray::IRLocalArrayElement::IRLocalArrayElement(emitters::IRFunctionEmitter& function, llvm::Value* data, llvm::Value* offset)
        : _function(function), _data(data), _offset(offset) {}

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
