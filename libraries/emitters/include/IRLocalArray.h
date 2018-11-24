////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalArray.h (emitters)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRLocalScalar.h"
#include "LLVMUtilities.h"

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
            IRLocalArrayValue(IRFunctionEmitter& function, LLVMValue pPointer, LLVMValue pOffset);

            IRLocalArrayValue& operator=(const IRLocalArrayValue& value);

            IRLocalArrayValue& operator=(LLVMValue value);

            operator IRLocalScalar() const;

            IRFunctionEmitter& _function;
            LLVMValue _pPointer;
            LLVMValue _pOffset;
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
} // namespace emitters
} // namespace ell
