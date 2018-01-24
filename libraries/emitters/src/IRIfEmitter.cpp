////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRIfEmitter.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRIfEmitter.h"
#include "IRBlockRegion.h"
#include "IRFunctionEmitter.h"

namespace ell
{
namespace emitters
{
    const std::string IfCondBlockName = "if.cond";
    const std::string IfThenBlockName = "if.then";
    const std::string IfElseBlockName = "if.else";
    const std::string IfEndBlockName = "if.end";
    const std::string IfAfterBlockName = "if.after";

    IRIfEmitter::IRIfEmitter(IRFunctionEmitter& functionEmitter, llvm::BasicBlock* pPrevBlock)
        : IRIfEmitter(functionEmitter, false, pPrevBlock)
    {
    }

    IRIfEmitter::IRIfEmitter(IRFunctionEmitter& functionEmitter, TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue)
        : IRIfEmitter(functionEmitter)
    {
        If(comparison, pValue, pTestValue);
    }

    IRIfEmitter::IRIfEmitter(IRFunctionEmitter& functionEmitter, bool endOnDestruct, llvm::BasicBlock* pPrevBlock)
        : _functionEmitter(&functionEmitter), _pEndBlock(pPrevBlock), _endOnDestruct(endOnDestruct)
    {
        assert(_functionEmitter);

        _pAfterBlock = _functionEmitter->BlockAfter(GetParentBlock(), IfAfterBlockName);
    }

    // Move ctor and assignment op are needed to explicitly swap out values,
    // since the default behavior for "moving" fundamental types (aka, pointers) is to do a bitwise-copy
    IRIfEmitter::IRIfEmitter(IRIfEmitter&& other)
    {
        *this = std::move(other);
    }

    IRIfEmitter& IRIfEmitter::operator=(IRIfEmitter&& other)
    {
        if (this != &other)
        {
            std::swap(this->_endOnDestruct, other._endOnDestruct);
            std::swap(this->_functionEmitter, other._functionEmitter);
            std::swap(this->_pAfterBlock, other._pAfterBlock);
            std::swap(this->_pConditionBlock, other._pConditionBlock);
            std::swap(this->_pEndBlock, other._pEndBlock);
            std::swap(this->_pThenBlock, other._pThenBlock);
        }

        return *this;
    }

    IRIfEmitter::~IRIfEmitter()
    {
        if (_endOnDestruct)
        {
            End();
        }
    }

    IRIfEmitter& IRIfEmitter::Else(std::function<void(IRFunctionEmitter& function)> body)
    {
        Else();
        body(*_functionEmitter);
        return *this;
    }

    IRIfEmitter& IRIfEmitter::ElseIf(llvm::Value* pValue, std::function<void(IRFunctionEmitter& function)> body)
    {
        auto elseBlock = If(pValue);
        elseBlock->setName(IfElseBlockName);
        body(*_functionEmitter);
        return *this;
    }

    llvm::BasicBlock* IRIfEmitter::If(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue)
    {
        EndPrev();
        PrepareBlocks();
        IfThen(comparison, pValue, pTestValue);
        return _pThenBlock;
    }

    llvm::BasicBlock* IRIfEmitter::If(std::function<llvm::Value*()> comparison)
    {
        EndPrev();
        PrepareBlocks();
        _functionEmitter->SetCurrentBlock(_pConditionBlock);
        {
            _functionEmitter->Branch(comparison(), _pThenBlock, _pEndBlock);
        }
        _functionEmitter->SetCurrentBlock(_pThenBlock);
        return _pThenBlock;
    }

    llvm::BasicBlock* IRIfEmitter::IfElse(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue)
    {
        return If(comparison, pValue, pTestValue);
    }

    llvm::BasicBlock* IRIfEmitter::If(llvm::Value* pValue)
    {
        EndPrev();
        PrepareBlocks();
        _functionEmitter->SetCurrentBlock(_pConditionBlock);
        _functionEmitter->Branch(pValue, _pThenBlock, _pEndBlock);
        _functionEmitter->SetCurrentBlock(_pThenBlock);
        return _pThenBlock;
    }

    llvm::BasicBlock* IRIfEmitter::If(llvm::Value* pValue, bool testValue)
    {
        EndPrev();
        PrepareBlocks();
        _functionEmitter->SetCurrentBlock(_pConditionBlock);
        _functionEmitter->Branch(pValue, testValue, _pThenBlock, _pEndBlock);
        _functionEmitter->SetCurrentBlock(_pThenBlock);
        return _pThenBlock;
    }

    llvm::BasicBlock* IRIfEmitter::Else()
    {
        assert(_pEndBlock != nullptr); // Else was called twice, OR without a preceding If

        EndPrev();
        _pThenBlock = _pEndBlock;
        _pThenBlock->setName(IfElseBlockName);
        _pEndBlock = nullptr;
        _functionEmitter->SetCurrentBlock(_pThenBlock);
        return _pThenBlock;
    }

    void IRIfEmitter::End()
    {
        EndPrev();
        if (_pEndBlock != nullptr)
        {
            _functionEmitter->SetCurrentBlock(_pEndBlock);
            _functionEmitter->Branch(_pAfterBlock);
        }
        _functionEmitter->SetCurrentBlock(_pAfterBlock);
    }

    llvm::BasicBlock* IRIfEmitter::IfThenElse(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
    {
        EndPrev();
        PrepareBlocks(pThenBlock, pElseBlock);
        // Emit the conditional branch, then make the then block the current block
        IfThen(comparison, pValue, pTestValue);
        {
            // To branch the Then to the "after" block, first remove any dangling terminating branches in the "else" block
            _functionEmitter->DeleteTerminatingBranch();
        }
        Else();
        {
            // To branch the else to the "after" block, first remove any dangling terminating branches in the "else" block
            _functionEmitter->DeleteTerminatingBranch();
        }
        End();
        return _pConditionBlock;
    }

    llvm::BasicBlock* IRIfEmitter::IfThenElse(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue, IRBlockRegion* pThenRegion, IRBlockRegion* pElseRegion)
    {
        assert(pThenRegion != nullptr);
        assert(pElseRegion != nullptr);

        EndPrev();
        PrepareBlocks(pThenRegion, pElseRegion);
        IfThen(comparison, pValue, pTestValue);
        {
            _pThenBlock = pThenRegion->End();
            _functionEmitter->SetCurrentBlock(_pThenBlock);
        }
        _pEndBlock = pElseRegion->End();
        Else();
        End();
        return _pConditionBlock;
    }

    void IRIfEmitter::EndPrev()
    {
        if (_pThenBlock != nullptr)
        {
            auto pCurrentBlock = _functionEmitter->GetCurrentBlock();
            _functionEmitter->SetCurrentBlock((pCurrentBlock == _pThenBlock) ? _pThenBlock : pCurrentBlock);
            {
                _functionEmitter->Branch(_pAfterBlock);
            }
        }
    }

    void IRIfEmitter::IfThen(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue)
    {
        _functionEmitter->SetCurrentBlock(_pConditionBlock);
        _functionEmitter->Branch(comparison, pValue, pTestValue, _pThenBlock, _pEndBlock);
        _functionEmitter->SetCurrentBlock(_pThenBlock);
    }

    void IRIfEmitter::IfThen(llvm::Value* pValue)
    {
        _functionEmitter->SetCurrentBlock(_pConditionBlock);
        _functionEmitter->Branch(pValue, _pThenBlock, _pEndBlock);
        _functionEmitter->SetCurrentBlock(_pThenBlock);
    }

    void IRIfEmitter::PrepareBlocks()
    {
        _pConditionBlock = GetParentBlock();
        _pThenBlock = _functionEmitter->BlockAfter(_pConditionBlock, IfThenBlockName);
        _pEndBlock = _functionEmitter->BlockAfter(_pThenBlock, IfEndBlockName);
    }

    void IRIfEmitter::PrepareBlocks(llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
    {
        _pConditionBlock = GetParentBlock();
        if (pThenBlock != nullptr)
        {
            _pThenBlock = _functionEmitter->BlockAfter(_pConditionBlock, pThenBlock);
        }
        else
        {
            _pThenBlock = _functionEmitter->BlockAfter(_pConditionBlock, IfThenBlockName);
        }
        if (pElseBlock != nullptr)
        {
            _pEndBlock = _functionEmitter->BlockAfter(pThenBlock, pElseBlock);
        }
        else
        {
            _pEndBlock = _functionEmitter->BlockAfter(_pThenBlock, IfEndBlockName);
        }
    }

    void IRIfEmitter::PrepareBlocks(IRBlockRegion* pThenRegion, IRBlockRegion* pElseRegion)
    {
        // Insert the condition block before the start of the thenRegion
        _pConditionBlock = _functionEmitter->BlockBefore(pThenRegion->Start(), IfCondBlockName);
        // place the block that contains the "After" code of the "if-then" to after the Else region
        _functionEmitter->BlockAfter(pElseRegion->End(), _pAfterBlock);
        _pThenBlock = pThenRegion->Start();
        _pEndBlock = pElseRegion->Start();
    }

    llvm::BasicBlock* IRIfEmitter::GetParentBlock()
    {
        return (_pEndBlock != nullptr) ? _pEndBlock : _functionEmitter->GetCurrentBlock();
    }
}
}
