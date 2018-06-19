////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalArray.h (emitters)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRLocalScalar.h"

namespace llvm
{
    class Value;
}

namespace ell
{
namespace emitters
{
    /// <summary>
    /// Helper type for llvm values representing 1D array values local to a function
    /// </summary>
    struct IRLocalArray : public IRLocalValue
    {
    private:
        /// <summary>
        /// Helper type for llvm values representing values within arrays local to a function
        /// </summary>
        struct IRLocalArrayValue
        {
            IRLocalArrayValue(IRFunctionEmitter& function, llvm::Value* pPointer, llvm::Value* pOffset);

            IRLocalArrayValue& operator=(const IRLocalArrayValue& value);

            IRLocalArrayValue& operator=(llvm::Value* value);

            operator IRLocalScalar() const;

            IRFunctionEmitter& _function;
            llvm::Value* _pPointer;
            llvm::Value* _pOffset;
        };

    public:
        using IRLocalValue::IRLocalValue;

        /// <summary> Indexing operator to return a reference to the specified offset </summary>
        ///
        /// <param name="offset"> The offset where the value lies within the wrapped array. </param>
        /// <return> An instance of IRLocalArray::IRLocalArrayValue to represent the value at the offset within the array </returns>
        IRLocalArrayValue operator[](IRLocalScalar offset) const;

        /// <summary> Indexing operator to return a reference to the specified offset </summary>
        ///
        /// <param name="offset"> The offset where the value lies within the wrapped array. </param>
        /// <return> An instance of IRLocalArray::IRLocalArrayValue to represent the value at the offset within the array </returns>
        IRLocalArrayValue operator[](int offset) const;
    };
}
}
