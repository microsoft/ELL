////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRVectorUtilities.tcc (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename ValueType, utilities::IsFloatingPoint<ValueType>>
    llvm::Value* FillVector(IRFunctionEmitter& function, llvm::VectorType* type, ValueType elementValue)
    {
        return llvm::ConstantFP::get(type, elementValue);
    }

    template <typename ValueType, utilities::IsIntegral<ValueType>>
    llvm::Value* FillVector(IRFunctionEmitter& function, llvm::VectorType* type, ValueType elementValue)
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
    llvm::Value* HorizontalVectorSum(IRFunctionEmitter& function, llvm::Value* vectorValue)
    {
        llvm::Type* type = vectorValue->getType();
        assert(type->isVectorTy() && "Must have vector type");
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
}
}
