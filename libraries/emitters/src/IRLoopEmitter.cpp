////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLoopEmitter.cpp (emitters)
//  Authors:  Umesh Madan, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRLoopEmitter.h"
#include "IRFunctionEmitter.h"

namespace ell
{
namespace emitters
{
    const std::string LoopInitBlockName = "loop.init";
    const std::string LoopConditionBlockName = "loop.cond";
    const std::string LoopBodyBlockName = "loop.body";
    const std::string LoopIncBlockName = "loop.inc";
    const std::string LoopAfterBlockName = "loop.after";

    IRLoopEmitter::IRLoopEmitter(IRFunctionEmitter& functionEmitter) :
        _functionEmitter(functionEmitter) {}

    void IRLoopEmitter::AddLoopMetadata(llvm::BranchInst* branch, bool unroll, bool vectorize)
    {
        // Add metadata
        auto& context = _functionEmitter.GetEmitter().GetContext();
        std::vector<llvm::Metadata*> metadataElements;

        // Reserve operand 0 for loop id self reference.
        auto tempNode = llvm::MDNode::getTemporary(context, {});
        metadataElements.push_back(tempNode.get());

        if (unroll)
        {
            llvm::Metadata* vals[] = { llvm::MDString::get(context, "llvm.loop.unroll.enable"),
                                       llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(
                                           llvm::Type::getInt1Ty(context), true)) };
            metadataElements.push_back(llvm::MDNode::get(context, vals));
        }

        if (vectorize)
        {
            llvm::Metadata* vals[] = { llvm::MDString::get(context, "llvm.loop.vectorize.enable"),
                                       llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(
                                           llvm::Type::getInt1Ty(context), true)) };
            metadataElements.push_back(llvm::MDNode::get(context, vals));
        }

        auto loopID = llvm::MDNode::get(context, metadataElements);
        loopID->replaceOperandWith(0, loopID);
        branch->setMetadata("llvm.loop", loopID);
    }

    // Blocks used in a for loop:
    //
    // _pInitializationBlock -- setup
    // _pConditionBlock -- termination check
    // _pBodyBlock -- body of the loop
    // _pIncrementBlock -- increment iteration variable and branch back to condition block
    // _pAfterBlock -- branch to this block when done
    //

    IRForLoopEmitter::IRForLoopEmitter(IRFunctionEmitter& functionEmitter) :
        IRLoopEmitter(functionEmitter) {}

    void IRForLoopEmitter::CreateBlocks()
    {
        _pInitializationBlock = _functionEmitter.Block(LoopInitBlockName);
        _pConditionBlock = _functionEmitter.Block(LoopConditionBlockName);
        _pBodyBlock = _functionEmitter.Block(LoopBodyBlockName);
        _pIncrementBlock = _functionEmitter.Block(LoopIncBlockName);
        _pAfterBlock = _functionEmitter.Block(LoopAfterBlockName);
    }

    llvm::BasicBlock* IRForLoopEmitter::Begin(int repeatCount)
    {
        return Begin(0, repeatCount, 1);
    }

    llvm::BasicBlock* IRForLoopEmitter::Begin(int iStartAt, int iMaxValue, int stepSize)
    {
        return Begin(_functionEmitter.Literal(iStartAt), _functionEmitter.Literal(iMaxValue), _functionEmitter.Literal(stepSize));
    }

    llvm::BasicBlock* IRForLoopEmitter::Begin(LLVMValue iStartAt, LLVMValue iMaxValue, LLVMValue stepSize)
    {
        CreateBlocks();
        EmitIterationVariable(iStartAt);
        EmitCondition(GetComparison(iMaxValue->getType(), BinaryPredicateType::less), iMaxValue);
        EmitIncrement(stepSize);
        return PrepareBody();
    }

    llvm::BasicBlock* IRForLoopEmitter::Begin(LLVMValue pRepeatCount)
    {
        if (pRepeatCount == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Repeat count must not be null");
        }

        return Begin(_functionEmitter.Literal(0), pRepeatCount, _functionEmitter.Literal(1));
    }

    LLVMValue IRForLoopEmitter::LoadIterationVariable()
    {
        return _functionEmitter.Load(_pIterationVariable);
    }

    void IRForLoopEmitter::EmitIterationVariable(LLVMValue pStartValue)
    {
        _functionEmitter.Branch(_pInitializationBlock);
        _functionEmitter.SetCurrentBlock(_pInitializationBlock);
        {
            _pIterationVariable = _functionEmitter.Variable(pStartValue->getType());
            _functionEmitter.Store(_pIterationVariable, pStartValue);
        }
    }

    void IRForLoopEmitter::EmitCondition(TypedComparison comparison, LLVMValue pTestValue)
    {
        _functionEmitter.Branch(_pConditionBlock);
        _functionEmitter.SetCurrentBlock(_pConditionBlock);
        auto branchInst = _functionEmitter.Branch(comparison, _functionEmitter.Load(_pIterationVariable), pTestValue, _pBodyBlock, _pAfterBlock);
        
        bool unroll = false;
        bool vectorize = false;
        AddLoopMetadata(branchInst, unroll, vectorize);
    }

    void IRForLoopEmitter::EmitIncrement(LLVMValue pIncrementValue)
    {
        auto isFp = pIncrementValue->getType()->isFloatingPointTy();
        _functionEmitter.SetCurrentBlock(_pIncrementBlock);
        _functionEmitter.OperationAndUpdate(_pIterationVariable,
                                            isFp ? TypedOperator::addFloat : TypedOperator::add,
                                            pIncrementValue);
        _functionEmitter.Branch(_pConditionBlock);
    }

    llvm::BasicBlock* IRForLoopEmitter::PrepareBody()
    {
        _functionEmitter.SetCurrentBlock(_pBodyBlock);
        return _pBodyBlock;
    }

    void IRForLoopEmitter::End()
    {
        if (_pIncrementBlock == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Got null increment block --- End() was probably called without first calling Begin()");
        }

        // Caller is done generating the body. Add a branch from the Body block to the increment block
        _functionEmitter.Branch(_pIncrementBlock);
        _functionEmitter.SetCurrentBlock(_pAfterBlock);
    }

    // Blocks used in a while loop:
    //
    // _pInitializationBlock -- setup
    // _pConditionBlock -- termination check
    // _pBodyBlock -- body of the loop
    // _pAfterBlock -- branch to this block when done
    //

    //
    // IRWhileLoopEmitter
    //
    IRWhileLoopEmitter::IRWhileLoopEmitter(IRFunctionEmitter& functionEmitter) :
        IRLoopEmitter(functionEmitter) {}

    void IRWhileLoopEmitter::CreateBlocks()
    {
        _pInitializationBlock = _functionEmitter.Block(LoopInitBlockName);
        _pConditionBlock = _functionEmitter.Block(LoopConditionBlockName);
        _pBodyBlock = _functionEmitter.Block(LoopBodyBlockName);
        _pAfterBlock = _functionEmitter.Block(LoopAfterBlockName);
    }

    llvm::BasicBlock* IRWhileLoopEmitter::Begin(LLVMValue pTestValuePointer)
    {
        if (pTestValuePointer == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Test value must not be null");
        }

        CreateBlocks();
        EmitInitialization();
        EmitCondition(pTestValuePointer);
        return PrepareBody();
    }

    llvm::BasicBlock* IRWhileLoopEmitter::Begin(std::function<LLVMValue(IRFunctionEmitter&)> condition)
    {
        CreateBlocks();
        EmitInitialization();
        EmitCondition(condition);
        return PrepareBody();
    }

    void IRWhileLoopEmitter::EmitInitialization()
    {
        _functionEmitter.Branch(_pInitializationBlock);
        _functionEmitter.SetCurrentBlock(_pInitializationBlock);
        {
            // Currently, no initialization needs to be done in a while loop
        }
    }

    void IRWhileLoopEmitter::EmitCondition(LLVMValue pTestValuePointer)
    {
        _functionEmitter.Branch(_pConditionBlock);
        _functionEmitter.SetCurrentBlock(_pConditionBlock);
        {
            _functionEmitter.Branch(_functionEmitter.Load(_functionEmitter.PointerOffset(pTestValuePointer, _functionEmitter.Literal(0))),
                                    _pBodyBlock,
                                    _pAfterBlock);
        }
    }

    void IRWhileLoopEmitter::EmitCondition(std::function<LLVMValue(IRFunctionEmitter&)> condition)
    {
        _functionEmitter.Branch(_pConditionBlock);
        _functionEmitter.SetCurrentBlock(_pConditionBlock);
        auto conditionValue = condition(_functionEmitter);
        auto branchInst = _functionEmitter.Branch(conditionValue,
                                                 _pBodyBlock,
                                                 _pAfterBlock);
        bool unroll = false;
        bool vectorize = false;
        AddLoopMetadata(branchInst, unroll, vectorize);
    }

    llvm::BasicBlock* IRWhileLoopEmitter::PrepareBody()
    {
        _functionEmitter.SetCurrentBlock(_pBodyBlock);
        return _pBodyBlock;
    }

    void IRWhileLoopEmitter::End()
    {
        // Caller is done generating the body. Add a branch from the Body block to the condition block
        _functionEmitter.Branch(_pConditionBlock);
        _functionEmitter.SetCurrentBlock(_pAfterBlock);
    }
} // namespace emitters
} // namespace ell
