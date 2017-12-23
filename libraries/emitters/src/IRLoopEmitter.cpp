////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLoopEmitter.cpp (emitters)
//  Authors:  Umesh Madan
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

    // Blocks used in a for loop:
    //
    // _pInitializationBlock -- setup
    // _pConditionBlock -- termination check
    // _pBodyBlock -- body of the loop
    // _pIncrementBlock -- increment iteration variable and branch back to condition block
    // _pAfterBlock -- branch to this block when done
    //

    IRForLoopEmitter::IRForLoopEmitter(IRFunctionEmitter& functionEmitter)
        : _functionEmitter(functionEmitter) {}

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
        CreateBlocks();
        EmitIterationVariable(VariableType::Int32, _functionEmitter.Literal(iStartAt));
        EmitCondition(TypedComparison::lessThan, _functionEmitter.Literal(iMaxValue));
        EmitIncrement(VariableType::Int32, _functionEmitter.Literal(stepSize));
        return PrepareBody();
    }

    llvm::BasicBlock* IRForLoopEmitter::Begin(llvm::Value* iStartAt, llvm::Value* iMaxValue, llvm::Value* stepSize)
    {
        CreateBlocks();
        EmitIterationVariable(VariableType::Int32, iStartAt);
        EmitCondition(TypedComparison::lessThan, iMaxValue);
        EmitIncrement(VariableType::Int32, stepSize);
        return PrepareBody();
    }

    llvm::BasicBlock* IRForLoopEmitter::Begin(llvm::Value* pRepeatCount)
    {
        assert(pRepeatCount != nullptr);

        CreateBlocks();
        EmitIterationVariable(VariableType::Int32, _functionEmitter.Literal(0));
        EmitCondition(TypedComparison::lessThan, pRepeatCount);
        EmitIncrement(VariableType::Int32, _functionEmitter.Literal(1));
        return PrepareBody();
    }

    llvm::Value* IRForLoopEmitter::LoadIterationVariable()
    {
        return _functionEmitter.Load(_pIterationVariable);
    }

    void IRForLoopEmitter::EmitIterationVariable(VariableType type, llvm::Value* pStartValue)
    {
        _functionEmitter.Branch(_pInitializationBlock);
        _functionEmitter.SetCurrentBlock(_pInitializationBlock);
        {
            _pIterationVariable = _functionEmitter.Variable(type);
            _functionEmitter.Store(_pIterationVariable, pStartValue);
        }
    }

    void IRForLoopEmitter::EmitCondition(TypedComparison type, llvm::Value* pTestValue)
    {
        _functionEmitter.Branch(_pConditionBlock);
        _functionEmitter.SetCurrentBlock(_pConditionBlock);
        {
            _functionEmitter.Branch(type, _functionEmitter.Load(_pIterationVariable), pTestValue, _pBodyBlock, _pAfterBlock); // TODO: add loop metadata to the branch instruction
        }
    }

    void IRForLoopEmitter::EmitIncrement(VariableType type, llvm::Value* pIncrementValue)
    {
        _functionEmitter.SetCurrentBlock(_pIncrementBlock);
        _functionEmitter.OperationAndUpdate(_pIterationVariable,
                                            (type == VariableType::Double) ? TypedOperator::addFloat : TypedOperator::add,
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
        assert(_pIncrementBlock != nullptr);

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
    IRWhileLoopEmitter::IRWhileLoopEmitter(IRFunctionEmitter& functionEmitter)
        : _functionEmitter(functionEmitter) {}

    void IRWhileLoopEmitter::CreateBlocks()
    {
        _pInitializationBlock = _functionEmitter.Block(LoopInitBlockName);
        _pConditionBlock = _functionEmitter.Block(LoopConditionBlockName);
        _pBodyBlock = _functionEmitter.Block(LoopBodyBlockName);
        _pAfterBlock = _functionEmitter.Block(LoopAfterBlockName);
    }

    llvm::BasicBlock* IRWhileLoopEmitter::Begin(llvm::Value* pTestValuePointer)
    {
        assert(pTestValuePointer != nullptr);

        CreateBlocks();
        EmitInitialization();
        EmitCondition(pTestValuePointer);
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

    void IRWhileLoopEmitter::EmitCondition(llvm::Value* pTestValuePointer)
    {
        _functionEmitter.Branch(_pConditionBlock);
        _functionEmitter.SetCurrentBlock(_pConditionBlock);
        {
            _functionEmitter.Branch(_functionEmitter.Load(_functionEmitter.PointerOffset(pTestValuePointer, _functionEmitter.Literal(0))),
                                    _pBodyBlock,
                                    _pAfterBlock);
        }
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
}
}