////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLoopEmitter.tcc (emitters)
//  Authors:  Umesh Madan, Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename ValueType, BinaryPredicateType predicate>
    llvm::BasicBlock* IRForLoopEmitter::Begin(llvm::Value* pStart, llvm::Value* pIncrement, llvm::Value* pTestValuePointer)
    {
        assert(pStart != nullptr && pIncrement != nullptr && pTestValuePointer != nullptr);

        auto type = GetVariableType<ValueType>();
        CreateBlocks();
        EmitIterationVariable(type, pStart);
        EmitMutableCondition(GetComparison<ValueType>(predicate), pTestValuePointer);
        EmitIncrement(type, pIncrement);
        return PrepareBody();
    }
}
}
