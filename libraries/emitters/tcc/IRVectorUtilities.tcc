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

    template <typename ValueType>
    llvm::Value* HorizontalVectorSum(IRFunctionEmitter& function, llvm::Value* vectorValue)
    {
        llvm::Type* type = vectorValue->getType();
        assert(type->isVectorTy() && "Must have vector type");
        llvm::VectorType* vecType = llvm::cast<llvm::VectorType>(type);
        assert(vecType != nullptr);

        int vectorSize = vecType->getNumElements();
        IREmitter& emitter = function.GetEmitter();
        llvm::Value* sum = function.Literal<ValueType>(0);
        for (int entryIndex = 0; entryIndex < vectorSize; ++entryIndex)
        {
            auto entryValue = emitter.GetIRBuilder().CreateExtractElement(vectorValue, entryIndex);
            sum = function.Operator(emitters::GetAddForValueType<ValueType>(), sum, entryValue);
        }
        return sum;
    }
    // TODO: implement logN version:
    //
    //        define i32 @sum(<8 x i32> %a) {
    //            %v1 = shufflevector <8 x i32> %a, <8 x i32> undef, <4 x i32> <i32 0, i32 1, i32 2, i32 3>  // extract elements 0, 1, 2, 3 into new 4-element vector
    //            %v2 = shufflevector <8 x i32> %a, <8 x i32> undef, <4 x i32> <i32 4, i32 5, i32 6, i32 7>  // extract elements 4, 5, 6, 7 into new 4-element vector
    //            %sum1 = add <4 x i32> %v1, %v2                                                             // sum them
    //            %v3 = shufflevector <4 x i32> %sum1, <4 x i32> undef, <2 x i32> <i32 0, i32 1>             // now extract elements 0 and 1 into new 2-element vector
    //            %v4 = shufflevector <4 x i32> %sum1, <4 x i32> undef, <2 x i32> <i32 2, i32 3>             // and extract elements 2 and 3 into new 2-element vector
    //            %sum2 = add <2 x i32> %v3, %v4                                                             // sum them
    //            %v5 = extractelement <2 x i32> %sum2, i32 0                                                // extract elements 0 and 1 into scalars
    //            %v6 = extractelement <2 x i32> %sum2, i32 1
    //            %sum3 = add i32 %v5, %v6                                                                   // and sum them for the result
    //            ret i32 %sum3
    //        }
}
}
