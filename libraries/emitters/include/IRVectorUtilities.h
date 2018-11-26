////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRVectorUtilities.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterTypes.h"
#include "IRFunctionEmitter.h"
#include "LLVMUtilities.h"

#include <utilities/include/TypeTraits.h>

namespace ell
{
namespace emitters
{
    /// <summary> Create an integer vector filled with copies of a value </summary>
    ///
    /// <typeparam name="ValueType"> The type of the value </typeparam>
    /// <param name="function"> The function being emitted </param>
    /// <param name="type"> The LLVM type of the result vector value </param>
    /// <param name="elementValue"> The value to place in the vector elements </param>
    ///
    /// <returns> An LLVM vector with repeated entries of the indicated value </returns>
    template <typename ValueType, utilities::IsIntegral<ValueType> = true>
    LLVMValue FillVector(IRFunctionEmitter& function, llvm::VectorType* type, ValueType elementValue);

    /// <summary> Create a floating-point vector filled with copies of a value </summary>
    ///
    /// <typeparam name="ValueType"> The type of the value </typeparam>
    /// <param name="function"> The function being emitted </param>
    /// <param name="type"> The LLVM type of the result vector value </param>
    /// <param name="elementValue"> The value to place in the vector elements </param>
    ///
    /// <returns> An LLVM vector with repeated entries of the indicated value </returns>
    template <typename ValueType, utilities::IsFloatingPoint<ValueType> = true>
    LLVMValue FillVector(IRFunctionEmitter& function, llvm::VectorType* type, ValueType elementValue);

    /// <summary> Compute the sum of the entries in a vector </summary>
    ///
    /// Emits explicit vector code to compute the sum. Hopefully, the vecorizing optimizer will
    /// take care of this when vecorizing simple loops to sum up values, but for other operations
    /// we may want to do it ourselves.
    ///
    /// <param name="function"> The function being emitted </param>
    /// <param name="vectorValue"> The value to sum the elements of </param>
    ///
    /// <returns> The sum of the elements in the given vector </returns>
    template <typename ValueType>
    LLVMValue HorizontalVectorSum(IRFunctionEmitter& function, LLVMValue vectorValue);
} // namespace emitters
} // namespace ell

#pragma region implementation

namespace ell
{
namespace emitters
{
    template <typename ValueType, utilities::IsFloatingPoint<ValueType>>
    LLVMValue FillVector(IRFunctionEmitter& function, llvm::VectorType* type, ValueType elementValue)
    {
        return llvm::ConstantFP::get(type, elementValue);
    }

    template <typename ValueType, utilities::IsIntegral<ValueType>>
    LLVMValue FillVector(IRFunctionEmitter& function, llvm::VectorType* type, ValueType elementValue)
    {
        return llvm::ConstantInt::get(type, elementValue, true);
    }

    // Emit explicit vectorized code to compute the sum of all the elements in a vector.
    // Hopefully, the vecorizing optimizer will take care of this when vecorizing simple
    // loops to sum up values, but for other operations we may want to do it ourselves.
    //
    // Runs in logN time by recursively splitting the vector in half and summing the halves.
    // Example:
    //   <1, 2, 3, 4, 5, 6, 7, 8> --> <1, 2, 3, 4> + <5, 6, 7, 8>    ( == <6, 8, 10, 12> )
    //   <6, 8, 10, 12> --> <6, 8> + <10, 12>    ( == <16, 20> )
    //   <16, 20> --> 16 + 20    ( == 36 )
    template <typename ValueType>
    LLVMValue HorizontalVectorSum(IRFunctionEmitter& function, LLVMValue vectorValue)
    {
        LLVMType type = vectorValue->getType();

        // Allow calling HorizontalVectorSum to be a no-op on a scalar
        if (!type->isVectorTy())
        {
            return vectorValue;
        }

        llvm::VectorType* vecType = llvm::cast<llvm::VectorType>(type);
        assert(vecType != nullptr);

        int vectorSize = vecType->getNumElements();
        IREmitter& emitter = function.GetEmitter();

        // Take care of the edge case of 1-element vectors
        if (vectorSize == 1)
        {
            return emitter.GetIRBuilder().CreateExtractElement(vectorValue, static_cast<uint64_t>(0));
        }

        // Repeatedly split the vector into two halves, and add the two halves together
        auto undef = llvm::UndefValue::get(type); // This undef is to tell LLVM we don't care what goes in the second operand of the shufflevector instruction
        while (vectorSize > 2)
        {
            assert(vectorSize % 2 == 0); // vectorSize must be a power of 2
            std::vector<uint32_t> elementIndices1;
            std::vector<uint32_t> elementIndices2;
            for (int index = 0; index < vectorSize / 2; ++index)
            {
                elementIndices1.push_back(index); // Collect indices [0, vectorSize/2)
                elementIndices2.push_back((vectorSize / 2) + index); // Collect indices [vectorSize/2, vectorSize)
            }
            auto half1 = emitter.GetIRBuilder().CreateShuffleVector(vectorValue, undef, elementIndices1); // Extract elements [0, vectorSize/2)
            auto half2 = emitter.GetIRBuilder().CreateShuffleVector(vectorValue, undef, elementIndices2); // Extract elements [vectorSize/2, vectorSize)
            vectorValue = function.Operator(emitters::GetAddForValueType<ValueType>(), half1, half2);
            vectorSize /= 2;
        }

        assert(vectorSize == 2);
        auto half1 = emitter.GetIRBuilder().CreateExtractElement(vectorValue, static_cast<uint64_t>(0));
        auto half2 = emitter.GetIRBuilder().CreateExtractElement(vectorValue, static_cast<uint64_t>(1));
        return function.Operator(emitters::GetAddForValueType<ValueType>(), half1, half2);
    }
} // namespace emitters
} // namespace ell

#pragma endregion implementation
