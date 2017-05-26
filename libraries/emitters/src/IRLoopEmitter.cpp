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
    const std::string ForInitBlockName = "for.init";
    const std::string ForConditionBlockName = "for.cond";
    const std::string ForBodyBlockName = "for.body";
    const std::string ForIncBlockName = "for.inc";
    const std::string ForAfterBlockName = "for.after";

    void IRForLoopEmitter::End()
    {
        assert(_pIncrementBlock != nullptr);

        // Caller is done generating the body. Add a branch from the Body block to the increment block
        _functionEmitter.Branch(_pIncrementBlock);
        _functionEmitter.SetCurrentBlock(_pAfterBlock);
    }

    void IRForLoopEmitter::Clear()
    {
        _pInitializationBlock = nullptr;
        _pConditionBlock = nullptr;
        _pBodyBlock = nullptr;
        _pIncrementBlock = nullptr;
        _pAfterBlock = nullptr;
        _pIterationVariable = nullptr;
    }

    IRForLoopEmitter::IRForLoopEmitter(IRFunctionEmitter& functionEmitter)
        : _functionEmitter(functionEmitter) {}

    llvm::Value* IRForLoopEmitter::LoadIterationVariable()
    {
        return _functionEmitter.Load(_pIterationVariable);
    }

    llvm::BasicBlock* IRForLoopEmitter::Begin(const int repeatCount)
    {
        return Begin(0, repeatCount, 1);
    }

    llvm::BasicBlock* IRForLoopEmitter::Begin(const int iStartAt, const int iMaxValue, const int stepSize)
    {
        CreateBlocks();
        EmitIterationVariable(VariableType::Int32, _functionEmitter.Literal(iStartAt));
        EmitCondition(TypedComparison::lessThan, _functionEmitter.Literal(iMaxValue));
        EmitIncrement(VariableType::Int32, _functionEmitter.Literal(stepSize));
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

    void IRForLoopEmitter::CreateBlocks()
    {
        _pInitializationBlock = _functionEmitter.Block(ForInitBlockName);
        _pConditionBlock = _functionEmitter.Block(ForConditionBlockName);
        _pBodyBlock = _functionEmitter.Block(ForBodyBlockName);
        _pIncrementBlock = _functionEmitter.Block(ForIncBlockName);
        _pAfterBlock = _functionEmitter.Block(ForAfterBlockName);
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
            _functionEmitter.Branch(type, _functionEmitter.Load(_pIterationVariable), pTestValue, _pBodyBlock, _pAfterBlock);
        }
    }

    void IRForLoopEmitter::EmitMutableCondition(TypedComparison type, llvm::Value* pTestValuePointer)
    {
        _functionEmitter.Branch(_pConditionBlock);
        _functionEmitter.SetCurrentBlock(_pConditionBlock);
        {
            _functionEmitter.Branch(type, _functionEmitter.Load(_pIterationVariable),
                _functionEmitter.Load(_functionEmitter.PointerOffset(pTestValuePointer, _functionEmitter.Literal(0))),
                _pBodyBlock, _pAfterBlock);
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
}
}