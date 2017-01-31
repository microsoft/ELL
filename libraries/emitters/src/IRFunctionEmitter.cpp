////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionEmitter.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "EmitterException.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRFunctionEmitter.h"

namespace ell
{
namespace emitters
{
    const std::string PrintfFnName = "printf";
    const std::string MallocFnName = "malloc";
    const std::string FreeFnName = "free";

    IRFunctionEmitter::IRFunctionEmitter(IREmitter* pEmitter, llvm::Function* pFunction)
    {
        Initialize(pEmitter, pFunction);
    }

    IRFunctionEmitter::IRFunctionEmitter(const IRFunctionEmitter& other)
        : _pFunction(other._pFunction), _pEmitter(other._pEmitter)
    {
    }

    void IRFunctionEmitter::Initialize(IREmitter* pEmitter, llvm::Function* pFunction)
    {
        assert(pEmitter != nullptr);
        assert(pFunction != nullptr);
        _pEmitter = pEmitter;
        _pFunction = pFunction;
    }

    void IRFunctionEmitter::Complete(IRFunctionOptimizer* pOptimizer)
    {
        Verify();
        if (pOptimizer != nullptr)
        {
            pOptimizer->Run(_pFunction);
        }
        else
        {
            Optimize();
        }
    }

    void IRFunctionEmitter::Verify()
    {
        llvm::verifyFunction(*_pFunction);
    }

    llvm::Value* IRFunctionEmitter::LoadArgument(llvm::ilist_iterator<llvm::Argument>& argument)
    {
        return _pEmitter->Load(&(*argument));
    }

    llvm::Value* IRFunctionEmitter::Cast(llvm::Value* pValue, VariableType valueType)
    {
        return _pEmitter->Cast(pValue, valueType);
    }

    llvm::Value* IRFunctionEmitter::CastFloatToInt(llvm::Value* pValue)
    {
        return _pEmitter->CastFloatToInt(pValue, VariableType::Int32);
    }

    llvm::Value* IRFunctionEmitter::CastBoolToInt(llvm::Value* pValue)
    {
        return _pEmitter->CastInt(pValue, VariableType::Int32, false);
    }

    llvm::Value* IRFunctionEmitter::Call(const std::string& name, llvm::Value* pArgument)
    {
        llvm::Function* function = ResolveFunction(name);
        if (pArgument == nullptr)
        {
            return _pEmitter->Call(function);
        }
        return _pEmitter->Call(function, pArgument);
    }

    llvm::Value* IRFunctionEmitter::Call(const std::string& name, const IRValueList& arguments)
    {
        return _pEmitter->Call(ResolveFunction(name), arguments);
    }

    llvm::Value* IRFunctionEmitter::Call(const std::string& name, std::initializer_list<llvm::Value*> arguments)
    {
        _values.Replace(arguments);
        return _pEmitter->Call(ResolveFunction(name), _values);
    }

    llvm::Value* IRFunctionEmitter::Call(llvm::Function* pFunction, std::initializer_list<llvm::Value*> arguments)
    {
        assert(pFunction != nullptr);
        _values.Replace(arguments);
        return _pEmitter->Call(pFunction, _values);
    }

    void IRFunctionEmitter::Return()
    {
        _pEmitter->ReturnVoid();
    }

    llvm::Value* IRFunctionEmitter::Return(llvm::Value* value)
    {
        return _pEmitter->Return(value);
    }

    llvm::Value* IRFunctionEmitter::Operator(TypedOperator type, llvm::Value* pLeftValue, llvm::Value* pRightValue)
    {
        return _pEmitter->BinaryOperation(type, pLeftValue, pRightValue);
    }

    llvm::Value* IRFunctionEmitter::Operator(TypedOperator type, llvm::iterator_range<llvm::Function::arg_iterator>& arguments)
    {
        auto values = arguments.begin();
        auto l = &(*values);
        ++values;
        auto r = &(*values);
        return Operator(type, l, r);
    }

    void IRFunctionEmitter::VectorOperator(TypedOperator type, size_t size, llvm::Value* pLeftValue, llvm::Value* pRightValue, std::function<void(llvm::Value*, llvm::Value*)> aggregator)
    {
        assert(pLeftValue != nullptr);
        assert(pRightValue != nullptr);

        auto forLoop = ForLoop();
        forLoop.Begin(size);
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* pLeftItem = ValueAt(pLeftValue, i);
            llvm::Value* pRightItem = ValueAt(pRightValue, i);
            llvm::Value* pTemp = Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        }
        forLoop.End();
    }

    void IRFunctionEmitter::VectorOperator(TypedOperator type, llvm::Value* pSize, llvm::Value* pLeftValue, llvm::Value* pRightValue, std::function<void(llvm::Value*, llvm::Value*)> aggregator)
    {
        assert(pSize != nullptr);
        assert(pLeftValue != nullptr);
        assert(pRightValue != nullptr);

        auto forLoop = ForLoop();
        forLoop.Begin(pSize);
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* pLeftItem = ValueAt(pLeftValue, i);
            llvm::Value* pRightItem = ValueAt(pRightValue, i);
            llvm::Value* pTemp = Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        }
        forLoop.End();
    }

    void IRFunctionEmitter::VectorOperator(TypedOperator type, size_t size, llvm::Value* pLeftValue, int LeftStartAt, llvm::Value* pRightValue, int RightStartAt, std::function<void(llvm::Value*, llvm::Value*)> aggregator)
    {
        assert(pLeftValue != nullptr);
        assert(pRightValue != nullptr);

        auto forLoop = ForLoop();
        forLoop.Begin(size);
        {
            auto i = forLoop.LoadIterationVariable();

            llvm::Value* leftOffset = Operator(TypedOperator::add, i, Literal(LeftStartAt));
            llvm::Value* pLeftItem = ValueAt(pLeftValue, leftOffset);
            llvm::Value* rightOffset = Operator(TypedOperator::add, i, Literal(RightStartAt));
            llvm::Value* pRightItem = ValueAt(pRightValue, rightOffset);
            llvm::Value* pTemp = Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        }
        forLoop.End();
    }

    void IRFunctionEmitter::Branch(llvm::BasicBlock* pDestinationBlock)
    {
        assert(pDestinationBlock != nullptr);
        _pEmitter->Branch(pDestinationBlock);
    }

    void IRFunctionEmitter::Branch(llvm::Value* pConditionValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
    {
        _pEmitter->Branch(pConditionValue, pThenBlock, pElseBlock);
    }

    void IRFunctionEmitter::Branch(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
    {
        llvm::Value* pResult = Comparison(comparison, pValue, pTestValue);
        Branch(pResult, pThenBlock, pElseBlock);
    }

    void IRFunctionEmitter::Branch(llvm::Value* pValue, bool testValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
    {
        return Branch(_pEmitter->Comparison(pValue, testValue), pThenBlock, pElseBlock);
    }

    llvm::Value* IRFunctionEmitter::LogicalAnd(llvm::Value* pTestValue1, llvm::Value* pTestValue2)
    {
        assert(pTestValue1 != nullptr);
        assert(pTestValue2 != nullptr);

        llvm::Value* pResult = Variable(VariableType::Int32);
        IRIfEmitter ifEmitter = If();
        ifEmitter.If([&pTestValue1, this] { return _pEmitter->IsFalse(pTestValue1); });
        {
            Store(pResult, _pEmitter->False());
        }
        ifEmitter.If([&pTestValue2, this] { return _pEmitter->IsFalse(pTestValue2); });
        {
            Store(pResult, _pEmitter->False());
        }
        ifEmitter.Else();
        {
            Store(pResult, _pEmitter->True());
        }
        ifEmitter.End();
        return pResult;
    }

    llvm::Value* IRFunctionEmitter::LogicalOr(llvm::Value* pTestValue1, llvm::Value* pTestValue2)
    {
        assert(pTestValue1 != nullptr);
        assert(pTestValue2 != nullptr);

        llvm::Value* pResult = Variable(VariableType::Int32);
        IRIfEmitter ifEmitter = If();
        ifEmitter.If([&pTestValue1, this] { return _pEmitter->IsTrue(pTestValue1); });
        {
            Store(pResult, _pEmitter->True());
        }
        ifEmitter.If([&pTestValue2, this] { return _pEmitter->IsTrue(pTestValue2); });
        {
            Store(pResult, _pEmitter->True());
        }
        ifEmitter.Else();
        {
            Store(pResult, _pEmitter->False());
        }
        ifEmitter.End();
        return pResult;
    }

    void IRFunctionEmitter::DeleteTerminatingBranch()
    {
        auto pTerm = GetCurrentBlock()->getTerminator();
        if (pTerm != nullptr)
        {
            pTerm->eraseFromParent();
        }
    }

    llvm::Value* IRFunctionEmitter::Comparison(TypedComparison type, llvm::Value* pValue, llvm::Value* pTestValue)
    {
        return _pEmitter->Comparison(type, pValue, pTestValue);
    }

    llvm::BasicBlock* IRFunctionEmitter::BlockBefore(llvm::BasicBlock* pBlock, llvm::BasicBlock* pNewBlock)
    {
        assert(pNewBlock != nullptr);
        pNewBlock->removeFromParent();
        return _pEmitter->BlockBefore(_pFunction, pBlock, pNewBlock);
    }

    llvm::BasicBlock* IRFunctionEmitter::BlockAfter(llvm::BasicBlock* pBlock, const std::string& label)
    {
        return _pEmitter->BlockAfter(_pFunction, pBlock, label);
    }

    llvm::BasicBlock* IRFunctionEmitter::BlockAfter(llvm::BasicBlock* pBlock, llvm::BasicBlock* pNewBlock)
    {
        assert(pNewBlock != nullptr);
        pNewBlock->removeFromParent();
        return _pEmitter->BlockAfter(_pFunction, pBlock, pNewBlock);
    }

    void IRFunctionEmitter::BlocksAfter(llvm::BasicBlock* pBlock, const std::vector<llvm::BasicBlock*>& blocks)
    {
        llvm::BasicBlock* pPrevBlock = pBlock;
        for (auto pNewBlock : blocks)
        {
            BlockAfter(pPrevBlock, pNewBlock);
            pPrevBlock = pNewBlock;
        }
    }

    void IRFunctionEmitter::BlocksAfter(llvm::BasicBlock* pBlock, const IRBlockRegion* pRegion)
    {
        assert(pBlock != nullptr);
        assert(pRegion != nullptr);

        auto blocks = pRegion->ToVector();
        BlocksAfter(pBlock, blocks);
    }

    void IRFunctionEmitter::AppendBlock(llvm::BasicBlock* pBlock)
    {
        assert(pBlock != nullptr);
        _pFunction->getBasicBlockList().push_back(pBlock);
    }

    llvm::BasicBlock* IRFunctionEmitter::SetCurrentBlock(llvm::BasicBlock* pBlock)
    {
        llvm::BasicBlock* pCurrentBlock = GetCurrentBlock();
        _pEmitter->SetCurrentBlock(pBlock);
        return pCurrentBlock;
    }

    llvm::BasicBlock* IRFunctionEmitter::BeginBlock(const std::string& label, bool shouldConcatenate)
    {
        auto pBlock = Block(label);
        if (shouldConcatenate)
        {
            Branch(pBlock);
        }
        SetCurrentBlock(pBlock);
        return pBlock;
    }

    llvm::BasicBlock* IRFunctionEmitter::Block(const std::string& label)
    {
        return _pEmitter->Block(_pFunction, label);
    }

    llvm::BasicBlock* IRFunctionEmitter::BlockBefore(llvm::BasicBlock* pBlock, const std::string& label)
    {
        return _pEmitter->BlockBefore(_pFunction, pBlock, label);
    }

    void IRFunctionEmitter::ConcatBlocks(llvm::BasicBlock* pTopBlock, llvm::BasicBlock* pBottomBlock)
    {
        assert(pTopBlock != nullptr && pBottomBlock != nullptr);

        pBottomBlock->removeFromParent();
        _pEmitter->BlockAfter(_pFunction, pTopBlock, pBottomBlock);
        auto pPrevCurBlock = SetCurrentBlock(pTopBlock);
        {
            DeleteTerminatingBranch();
            Branch(pBottomBlock);
        }
        SetCurrentBlock(pPrevCurBlock);
    }

    void IRFunctionEmitter::MergeBlock(llvm::BasicBlock* pBlock)
    {
        assert(pBlock != nullptr);
        ConcatBlocks(GetCurrentBlock(), pBlock);
        SetCurrentBlock(pBlock);
    }

    void IRFunctionEmitter::MergeRegion(IRBlockRegion* pRegion)
    {
        assert(pRegion != nullptr);
        BlocksAfter(GetCurrentBlock(), pRegion);
        Branch(pRegion->Start());
        SetCurrentBlock(pRegion->End());
        pRegion->IsTopLevel() = false;
    }

    void IRFunctionEmitter::ConcatRegions(IRBlockRegion* pTop, IRBlockRegion* pBottom, bool moveBlocks)
    {
        assert(pTop != nullptr && pBottom != nullptr);

        if (moveBlocks)
        {
            BlocksAfter(pTop->End(), pBottom);
        }

        auto pPrevCurBlock = SetCurrentBlock(pTop->End());
        {
            DeleteTerminatingBranch();
            Branch(pBottom->Start());
            pTop->SetEnd(pBottom->End());
            pBottom->IsTopLevel() = false;
        }
        SetCurrentBlock(pPrevCurBlock);
    }

    void IRFunctionEmitter::ConcatRegions(IRBlockRegionList& regions)
    {
        IRBlockRegion* pPrevRegion = nullptr;
        for (size_t i = 0; i < regions.Size(); ++i)
        {
            IRBlockRegion* pRegion = regions.GetAt(i);
            if (pRegion->IsTopLevel())
            {
                if (pPrevRegion != nullptr)
                {
                    ConcatRegions(pPrevRegion, pRegion);
                }
                pPrevRegion = pRegion;
            }
        }
    }

    llvm::Value* IRFunctionEmitter::Variable(VariableType type)
    {
        return _pEmitter->Variable(type);
    }

    llvm::Value* IRFunctionEmitter::Variable(VariableType type, const std::string& name)
    {
        return _pEmitter->Variable(type, name);
    }

    llvm::Value* IRFunctionEmitter::Variable(VariableType type, int size)
    {
        return _pEmitter->StackAllocate(type, size);
    }

    llvm::Value* IRFunctionEmitter::Load(llvm::Value* pPointer)
    {
        return _pEmitter->Load(pPointer);
    }

    llvm::Value* IRFunctionEmitter::Load(llvm::Value* pPointer, const std::string& name)
    {
        return _pEmitter->Load(pPointer, name);
    }

    llvm::Value* IRFunctionEmitter::Store(llvm::Value* pPointer, llvm::Value* pValue)
    {
        return _pEmitter->Store(pPointer, pValue);
    }

    llvm::Value* IRFunctionEmitter::OperationAndUpdate(llvm::Value* pPointer, TypedOperator operation, llvm::Value* pValue)
    {
        llvm::Value* pNextVal = Operator(operation, Load(pPointer), pValue);
        Store(pPointer, pNextVal);
        return pNextVal;
    }

    llvm::Value* IRFunctionEmitter::PtrOffsetA(llvm::Value* pointer, int offset)
    {
        return _pEmitter->PointerOffset(pointer, Literal(offset));
    }

    llvm::Value* IRFunctionEmitter::PtrOffsetA(llvm::Value* pointer, llvm::Value* pOffset, const std::string& name)
    {
        return _pEmitter->PointerOffset(pointer, pOffset, name);
    }

    llvm::Value* IRFunctionEmitter::ValueAtA(llvm::Value* pointer, int offset)
    {
        return _pEmitter->Load(PtrOffsetA(pointer, offset));
    }

    llvm::Value* IRFunctionEmitter::ValueAtA(llvm::Value* pointer, llvm::Value* pOffset)
    {
        return _pEmitter->Load(PtrOffsetA(pointer, pOffset));
    }

    llvm::Value* IRFunctionEmitter::SetValueAtA(llvm::Value* pPointer, int offset, llvm::Value* pValue)
    {
        return _pEmitter->Store(PtrOffsetA(pPointer, offset), pValue);
    }

    llvm::Value* IRFunctionEmitter::SetValueAtA(llvm::Value* pPointer, llvm::Value* pOffset, llvm::Value* pValue)
    {
        return _pEmitter->Store(PtrOffsetA(pPointer, pOffset), pValue);
    }

    llvm::Value* IRFunctionEmitter::PtrOffsetH(llvm::Value* pointer, int offset)
    {
        return PtrOffsetH(pointer, Literal(offset));
    }

    llvm::Value* IRFunctionEmitter::PtrOffsetH(llvm::Value* pointer, llvm::Value* pOffset)
    {
        assert(pointer != nullptr);
        return _pEmitter->PointerOffset(Load(pointer), pOffset);
    }

    llvm::Value* IRFunctionEmitter::ValueAtH(llvm::Value* pointer, int offset)
    {
        return _pEmitter->Load(PtrOffsetH(pointer, offset));
    }

    llvm::Value* IRFunctionEmitter::ValueAtH(llvm::Value* pointer, llvm::Value* pOffset)
    {
        return _pEmitter->Load(PtrOffsetH(pointer, pOffset));
    }

    llvm::Value* IRFunctionEmitter::SetValueAtH(llvm::Value* pPointer, int offset, llvm::Value* pValue)
    {
        return _pEmitter->Store(PtrOffsetH(pPointer, offset), pValue);
    }

    llvm::Value* IRFunctionEmitter::Pointer(llvm::GlobalVariable* pGlobal)
    {
        return _pEmitter->Pointer(pGlobal);
    }

    llvm::Value* IRFunctionEmitter::PointerOffset(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset)
    {
        return _pEmitter->PointerOffset(pGlobal, pOffset);
    }

    llvm::Value* IRFunctionEmitter::PointerOffset(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pFieldOffset)
    {
        return _pEmitter->PointerOffset(pGlobal, pOffset, pFieldOffset);
    }

    llvm::Value* IRFunctionEmitter::ValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset)
    {
        return Load(_pEmitter->PointerOffset(pGlobal, pOffset));
    }

    llvm::Value* IRFunctionEmitter::PointerOffset(llvm::Value* pPointer, llvm::Value* pOffset)
    {
        llvm::GlobalVariable* pGlobal = llvm::dyn_cast<llvm::GlobalVariable>(pPointer);
        if (pGlobal != nullptr)
        {
            return PointerOffset(pGlobal, pOffset);
        }
        return PtrOffsetA(pPointer, pOffset);
    }

    llvm::Value* IRFunctionEmitter::PointerOffset(llvm::Value* pPointer, int offset)
    {
        return PointerOffset(pPointer, Literal(offset));
    }

    llvm::Value* IRFunctionEmitter::ValueAt(llvm::Value* pPointer, llvm::Value* pOffset)
    {
        llvm::GlobalVariable* pGlobal = llvm::dyn_cast<llvm::GlobalVariable>(pPointer);
        if (pGlobal != nullptr)
        {
            return ValueAt(pGlobal, pOffset);
        }
        return ValueAtA(pPointer, pOffset);
    }

    llvm::Value* IRFunctionEmitter::ValueAt(llvm::Value* pPointer, int offset)
    {
        return ValueAt(pPointer, Literal(offset));
    }

    llvm::Value* IRFunctionEmitter::SetValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pValue)
    {
        return Store(PointerOffset(pGlobal, pOffset), pValue);
    }

    llvm::Value* IRFunctionEmitter::SetValueAt(llvm::Value* pPointer, llvm::Value* pOffset, llvm::Value* pValue)
    {
        llvm::GlobalVariable* pGlobal = llvm::dyn_cast<llvm::GlobalVariable>(pPointer);
        if (pGlobal != nullptr)
        {
            return SetValueAt(pGlobal, pOffset, pValue);
        }
        return SetValueAtA(pPointer, pOffset, pValue);
    }

    llvm::Value* IRFunctionEmitter::SetValueAt(llvm::Value* pPointer, int offset, llvm::Value* pValue)
    {
        return SetValueAt(pPointer, Literal(offset), pValue);
    }

    IRForLoopEmitter IRFunctionEmitter::ForLoop()
    {
        return IRForLoopEmitter(*this);
    }

    IRIfEmitter IRFunctionEmitter::If()
    {
        return IRIfEmitter(*this);
    }

    IRIfEmitter IRFunctionEmitter::If(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue)
    {
        return IRIfEmitter(*this, comparison, pValue, pTestValue);
    }

    void IRFunctionEmitter::Optimize()
    {
        IRFunctionOptimizer optimizer(GetLLVMModule());
        optimizer.AddStandardPasses();
        Optimize(optimizer);
    }

    void IRFunctionEmitter::Optimize(IRFunctionOptimizer& optimizer)
    {
        optimizer.Run(GetFunction());
    }

    llvm::Value* IRFunctionEmitter::Malloc(VariableType type, int64_t size)
    {
        _values.Replace({ Literal(size) });
        return Cast(Call(MallocFnName, _values), type);
    }

    void IRFunctionEmitter::Free(llvm::Value* pValue)
    {
        Call(FreeFnName, Cast(pValue, VariableType::BytePointer));
    }

    llvm::Value* IRFunctionEmitter::Print(const std::string& text)
    {
        return Printf({ Literal(text) });
    }

    llvm::Value* IRFunctionEmitter::Printf(std::initializer_list<llvm::Value*> arguments)
    {
        return Call(PrintfFnName, arguments);
    }

    llvm::Value* IRFunctionEmitter::Printf(const std::string& format, std::initializer_list<llvm::Value*> arguments)
    {
        IRValueList callArgs;
        callArgs.Append(_pEmitter->Literal(format));
        callArgs.Append(arguments);
        return Call(PrintfFnName, callArgs);
    }

    void IRFunctionEmitter::PrintForEach(const std::string& formatString, llvm::Value* pVector, int size)
    {
        llvm::Value* pFormat = Literal(formatString);
        IRForLoopEmitter forLoop = ForLoop();
        forLoop.Begin(size);
        {
            auto ival = forLoop.LoadIterationVariable();
            auto v = ValueAt(pVector, ival);
            Printf({ pFormat, v });
        }
        forLoop.End();
    }

    llvm::Value* IRFunctionEmitter::DotProductFloat(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue)
    {
        llvm::Value* pTotal = Variable(VariableType::Double);
        DotProductFloat(size, pLeftValue, pRightValue, pTotal);
        return pTotal;
    }

    void IRFunctionEmitter::DotProductFloat(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination)
    {
        Store(pDestination, Literal(0.0));
        VectorOperator(TypedOperator::multiplyFloat, size, pLeftValue, pRightValue, [&pDestination, this](llvm::Value* i, llvm::Value* pValue) {
            OperationAndUpdate(pDestination, TypedOperator::addFloat, pValue);
        });
    }

    void IRFunctionEmitter::DotProductFloat(llvm::Value* pSize, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination)
    {
        Store(pDestination, Literal(0.0));
        VectorOperator(TypedOperator::multiplyFloat, pSize, pLeftValue, pRightValue, [&pDestination, this](llvm::Value* i, llvm::Value* pValue) {
            OperationAndUpdate(pDestination, TypedOperator::addFloat, pValue);
        });
    }

    llvm::Value* IRFunctionEmitter::DotProduct(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue)
    {
        llvm::Value* pTotal = Variable(VariableType::Int32);
        DotProductFloat(size, pLeftValue, pRightValue, pTotal);
        return pTotal;
    }

    void IRFunctionEmitter::DotProduct(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination)
    {
        Store(pDestination, Literal(0));
        VectorOperator(TypedOperator::multiply, size, pLeftValue, pRightValue, [&pDestination, this](llvm::Value* i, llvm::Value* pValue) {
            OperationAndUpdate(pDestination, TypedOperator::add, pValue);
        });
    }

    void IRFunctionEmitter::DotProduct(llvm::Value* pSize, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination)
    {
        Store(pDestination, Literal(0));
        VectorOperator(TypedOperator::multiply, pSize, pLeftValue, pRightValue, [&pDestination, this](llvm::Value* i, llvm::Value* pValue) {
            OperationAndUpdate(pDestination, TypedOperator::add, pValue);
        });
    }

    llvm::Function* IRFunctionEmitter::ResolveFunction(const std::string& name)
    {
        llvm::Function* pFunction = GetLLVMModule()->getFunction(name);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return pFunction;
    }

    void IRFunctionEmitter::Dump()
    {
        WriteToStream(std::cout);
    }

    void IRFunctionEmitter::WriteToStream(std::ostream& os)
    {
        llvm::raw_os_ostream out(os);
        _pFunction->print(out);
    }

    IRFunctionCallArguments::IRFunctionCallArguments(IRFunctionEmitter& caller)
        : _functionEmitter(caller)
    {
    }

    llvm::Value* IRFunctionCallArguments::GetArgumentAt(size_t index) const
    {
        return _arguments[index];
    }

    void IRFunctionCallArguments::Append(llvm::Value* pValue)
    {
        assert(pValue != nullptr);
        if (pValue->getType()->isPointerTy())
        {
            _arguments.Append(_functionEmitter.PointerOffset(pValue, 0));
        }
        else
        {
            _arguments.Append(pValue);
        }
    }

    llvm::Value* IRFunctionCallArguments::AppendOutput(VariableType valueType, int size)
    {
        llvm::Value* pVector = _functionEmitter.Variable(valueType, size);
        Append(pVector);
        return pVector;
    }
}
}
