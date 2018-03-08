////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionEmitter.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRFunctionEmitter.h"
#include "EmitterException.h"
#include "IRAsyncTask.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRMetadata.h"
#include "IRModuleEmitter.h"
#include "IRThreadPool.h"
#include "LLVMUtilities.h"

// utilities
#include "Logger.h"

// llvm
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>

namespace ell
{
namespace emitters
{
    using namespace utilities::logging;
    using utilities::logging::Log;

    const std::string PrintfFnName = "printf";
    const std::string MallocFnName = "malloc";
    const std::string FreeFnName = "free";

    IRFunctionEmitter::IRFunctionEmitter(IRModuleEmitter* pModuleEmitter, IREmitter* pEmitter, llvm::Function* pFunction, const std::string& name)
        : _pModuleEmitter(pModuleEmitter), _pEmitter(pEmitter), _pFunction(pFunction), _name(name)
    {
        assert(_pModuleEmitter != nullptr);
        assert(_pEmitter != nullptr);
        assert(_pFunction != nullptr);
        SetUpFunction();
    }

    IRFunctionEmitter::IRFunctionEmitter(IRModuleEmitter* pModuleEmitter, IREmitter* pEmitter, llvm::Function* pFunction, const NamedVariableTypeList& arguments, const std::string& name)
        : IRFunctionEmitter(pModuleEmitter, pEmitter, pFunction, name)
    {
        // Note: already called other constructor
        RegisterFunctionArgs(arguments);
    }

    IRFunctionEmitter::IRFunctionEmitter(IRModuleEmitter* pModuleEmitter, IREmitter* pEmitter, llvm::Function* pFunction, const NamedLLVMTypeList& arguments, const std::string& name)
        : IRFunctionEmitter(pModuleEmitter, pEmitter, pFunction, name)
    {
        // Note: already called other constructor
        RegisterFunctionArgs(arguments);
    }

    void IRFunctionEmitter::CompleteFunction(bool optimize)
    {
        Verify();
        if (optimize)
        {
            Optimize();
        }
    }

    void IRFunctionEmitter::CompleteFunction(IRFunctionOptimizer& optimizer)
    {
        Verify();
        Optimize(optimizer);
    }

    void IRFunctionEmitter::SetUpFunction()
    {
        // Set us up with a default entry point, since we'll always need one
        // We may add additional annotations here
        auto pBlock = Block("entry");
        AddRegion(pBlock);
        _pEmitter->SetCurrentBlock(pBlock); // if/when we get our own IREmitter, this statefulness won't be so objectionable
        _entryBlock = pBlock;
    }

    IRLocalScalar IRFunctionEmitter::LocalScalar(llvm::Value* value)
    {
        return IRLocalScalar(*this, value);
    }

    IRLocalScalar IRFunctionEmitter::LocalScalar()
    {
        return IRLocalScalar(*this, static_cast<llvm::Value*>(nullptr));
    }
    
    IRLocalArray IRFunctionEmitter::LocalArray(llvm::Value* value)
    {
        return IRLocalArray(*this, value);
    }

    llvm::Value* IRFunctionEmitter::GetEmittedVariable(const VariableScope scope, const std::string& name)
    {
        switch (scope)
        {
        case VariableScope::local:
        case VariableScope::input:
        case VariableScope::output:
            return _locals.Get(name);

        default:
            return GetModule().GetEmittedVariable(scope, name);
        }
    }

    llvm::Value* IRFunctionEmitter::GetFunctionArgument(const std::string& name)
    {
        return _locals.Get(name);
    }

    void IRFunctionEmitter::Verify()
    {
        llvm::verifyFunction(*_pFunction);
    }

    llvm::Value* IRFunctionEmitter::LoadArgument(llvm::ilist_iterator<llvm::Argument>& argument)
    {
        return _pEmitter->Load(&(*argument));
    }

    llvm::Value* IRFunctionEmitter::LoadArgument(llvm::Argument& argument)
    {
        return _pEmitter->Load(&argument);
    }

    llvm::Value* IRFunctionEmitter::BitCast(llvm::Value* pValue, VariableType valueType)
    {
        return _pEmitter->BitCast(pValue, valueType);
    }

    llvm::Value* IRFunctionEmitter::BitCast(llvm::Value* pValue, llvm::Type* valueType)
    {
        return _pEmitter->BitCast(pValue, valueType);
    }

    llvm::Value* IRFunctionEmitter::CastPointer(llvm::Value* pValue, llvm::Type* valueType)
    {
        return _pEmitter->CastPointer(pValue, valueType);
    }

    llvm::Value* IRFunctionEmitter::CastPointer(llvm::Value* pValue, VariableType valueType)
    {
        return _pEmitter->CastPointer(pValue, valueType);
    }

    llvm::Value* IRFunctionEmitter::CastIntToPointer(llvm::Value* pValue, llvm::Type* valueType)
    {
        return _pEmitter->CastIntToPointer(pValue, valueType);
    }

    llvm::Value* IRFunctionEmitter::CastPointerToInt(llvm::Value* pValue, llvm::Type* destinationType)
    {
        return _pEmitter->CastPointerToInt(pValue, destinationType);
    }

    llvm::Value* IRFunctionEmitter::CastIntToFloat(llvm::Value* pValue, VariableType destinationType, bool isSigned)
    {
        return _pEmitter->CastIntToFloat(pValue, destinationType, isSigned);
    }

    llvm::Value* IRFunctionEmitter::CastFloatToInt(llvm::Value* pValue, VariableType destinationType)
    {
        return _pEmitter->CastFloatToInt(pValue, destinationType);
    }

    llvm::Value* IRFunctionEmitter::CastBoolToByte(llvm::Value* pValue)
    {
        return _pEmitter->CastInt(pValue, VariableType::Byte, false);
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
        return _pEmitter->Call(ResolveFunction(name), arguments);
    }

    llvm::Value* IRFunctionEmitter::Call(IRFunctionEmitter& function, std::vector<llvm::Value*> arguments)
    {
        assert(function.GetFunction() != nullptr);
        return _pEmitter->Call(function.GetFunction(), arguments);
    }

    llvm::Value* IRFunctionEmitter::Call(llvm::Function* pFunction, std::initializer_list<llvm::Value*> arguments)
    {
        assert(pFunction != nullptr);
        return _pEmitter->Call(pFunction, arguments);
    }

    llvm::Value* IRFunctionEmitter::Call(llvm::Function* pFunction, std::vector<llvm::Value*> arguments)
    {
        assert(pFunction != nullptr);
        return _pEmitter->Call(pFunction, arguments);
    }

    void IRFunctionEmitter::Return()
    {
        _pEmitter->ReturnVoid();
    }

    llvm::Value* IRFunctionEmitter::Return(llvm::Value* value)
    {
        return _pEmitter->Return(value);
    }

    llvm::Value* IRFunctionEmitter::Operator(UnaryOperationType type, llvm::Value* value)
    {
        return _pEmitter->UnaryOperation(type, value);
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
        return Operator(TypedOperator::logicalAnd, pTestValue1, pTestValue2);
    }

    llvm::Value* IRFunctionEmitter::LogicalOr(llvm::Value* pTestValue1, llvm::Value* pTestValue2)
    {
        assert(pTestValue1 != nullptr);
        assert(pTestValue2 != nullptr);
        return Operator(TypedOperator::logicalOr, pTestValue1, pTestValue2);
    }

    llvm::Value* IRFunctionEmitter::LogicalNot(llvm::Value* pTestValue)
    {
        assert(pTestValue != nullptr);
        return _pEmitter->IsFalse(pTestValue);
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

    llvm::Value* IRFunctionEmitter::Select(llvm::Value* pCmp, llvm::Value* pTrueValue, llvm::Value* pFalseValue)
    {
        return _pEmitter->Select(pCmp, pTrueValue, pFalseValue);
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

    void IRFunctionEmitter::SetCurrentInsertPoint(llvm::IRBuilder<>::InsertPoint position)
    {
        _pEmitter->SetCurrentInsertPoint(position);
    }

    void IRFunctionEmitter::SetCurrentInsertPoint(llvm::Instruction* position)
    {
        _pEmitter->SetCurrentInsertPoint(position);
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

    void IRFunctionEmitter::ConcatenateBlocks(std::vector<llvm::BasicBlock*> blocks)
    {
        llvm::BasicBlock* previousBlock = nullptr;
        for (auto ptr = blocks.begin(), end = blocks.end(); ptr != end; ptr++)
        {
            llvm::BasicBlock* nextBlock = *ptr;
            if (previousBlock != nullptr)
            {
                ConcatenateBlocks(previousBlock, nextBlock);
            }
            previousBlock = nextBlock;
        }
    }

    void IRFunctionEmitter::ConcatenateBlocks(llvm::BasicBlock* pTopBlock, llvm::BasicBlock* pBottomBlock)
    {
        assert(pTopBlock != nullptr && pBottomBlock != nullptr);

        pBottomBlock->removeFromParent();
        _pEmitter->BlockAfter(_pFunction, pTopBlock, pBottomBlock);
        auto pPrevCurBlock = SetCurrentBlock(pTopBlock);
        {
            auto termInst = pTopBlock->getTerminator();
            if (termInst == nullptr)
            {
                Branch(pBottomBlock);
            }
        }
        SetCurrentBlock(pPrevCurBlock);
    }

    void IRFunctionEmitter::MergeBlock(llvm::BasicBlock* pBlock)
    {
        assert(pBlock != nullptr);
        ConcatenateBlocks(GetCurrentBlock(), pBlock);
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
        // using utilities::logging::Log;
        assert(pTop != nullptr && pBottom != nullptr);

        Log() << "Concatenating block regions";
        if (moveBlocks)
        {
            Log() << " and placing them together";
            BlocksAfter(pTop->End(), pBottom);
        }
        Log() << EOL;

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

    void IRFunctionEmitter::ConcatRegions()
    {
        ConcatRegions(_regions);
    }

    IRFunctionEmitter::EntryBlockScope::EntryBlockScope(IRFunctionEmitter& function)
        : _function(function)
    {
        // Save current position
        _oldPos = function.GetCurrentInsertPoint();

        auto entryBlock = function.GetEntryBlock();
        auto termInst = entryBlock->getTerminator();
        // If the function entry block contains a terminator, set the insert point
        // to be just _before_ the terminator. Otherwise, set the insert point
        // to be in the entry block.
        if (termInst != nullptr)
        {
            function.SetCurrentInsertPoint(termInst);
        }
        else
        {
            function.SetCurrentBlock(entryBlock);
        }
    }

    IRFunctionEmitter::EntryBlockScope::~EntryBlockScope()
    {
        _function.SetCurrentInsertPoint(_oldPos);
    }

    llvm::AllocaInst* IRFunctionEmitter::Variable(VariableType type)
    {
        EntryBlockScope scope(*this);
        return _pEmitter->StackAllocate(type);
    }

    llvm::AllocaInst* IRFunctionEmitter::Variable(VariableType type, const std::string& namePrefix)
    {
        EntryBlockScope scope(*this);

        // don't do this for emitted variables!
        auto name = _locals.GetUniqueName(namePrefix);
        auto result = _pEmitter->StackAllocate(type, name);
        _locals.Add(name, result);
        return result;
    }

    llvm::AllocaInst* IRFunctionEmitter::Variable(llvm::Type* type, const std::string& namePrefix)
    {
        EntryBlockScope scope(*this);
        auto name = _locals.GetUniqueName(namePrefix);
        auto result = _pEmitter->StackAllocate(type, name);
        _locals.Add(name, result);
        return result;
    }

    llvm::AllocaInst* IRFunctionEmitter::EmittedVariable(VariableType type, const std::string& name)
    {
        EntryBlockScope scope(*this);
        auto result = _pEmitter->StackAllocate(type, name);
        _locals.Add(name, result);
        return result;
    }

    llvm::AllocaInst* IRFunctionEmitter::Variable(VariableType type, int size)
    {
        EntryBlockScope scope(*this);
        return _pEmitter->StackAllocate(type, size);
    }

    llvm::AllocaInst* IRFunctionEmitter::Variable(llvm::Type* type, int size)
    {
        EntryBlockScope scope(*this);
        return _pEmitter->StackAllocate(type, size);
    }

    llvm::Value* IRFunctionEmitter::Load(llvm::Value* pPointer)
    {
        auto result = _pEmitter->Load(pPointer);
        return result;
    }

    llvm::Value* IRFunctionEmitter::Load(llvm::Value* pPointer, const std::string& name)
    {
        return _pEmitter->Load(pPointer, name);
    }

    llvm::Value* IRFunctionEmitter::Store(llvm::Value* pPointer, llvm::Value* pValue)
    {
        return _pEmitter->Store(pPointer, pValue);
    }

    llvm::Value* IRFunctionEmitter::StoreZero(llvm::Value* pPointer, int numElements /* = 1 */)
    {
        assert(numElements >= 1);
        assert(llvm::dyn_cast<llvm::GlobalVariable>(pPointer) == nullptr && "StoreZero can't handle llvm::GlobalVariables");

        auto type = llvm::cast<llvm::PointerType>(pPointer->getType())->getElementType();

        auto zero = llvm::Constant::getNullValue(type);
        llvm::Value* returnValue = nullptr;

        for (int index = 0; index < numElements; ++index)
        {
            // We use SetValueAtA directly because SetValueAt tries to see if the variable
            // is a global variable first, which is not handled by this function.
            returnValue = SetValueAtA(pPointer, index, zero);
        }

        return returnValue;
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

    void IRFunctionEmitter::FillStruct(llvm::Value* structPtr, const std::vector<llvm::Value*>& fieldValues)
    {
        auto& emitter = GetEmitter();
        auto& irBuilder = emitter.GetIRBuilder();
        for (size_t index = 0; index < fieldValues.size(); ++index)
        {
            auto field = irBuilder.CreateInBoundsGEP(structPtr, { Literal(0), Literal(static_cast<int>(index)) });
            Store(field, fieldValues[index]);
        }
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

    llvm::Value* IRFunctionEmitter::PointerOffset(llvm::GlobalVariable* pGlobal, int offset)
    {
        return _pEmitter->PointerOffset(pGlobal, Literal(offset));
    }

    llvm::Value* IRFunctionEmitter::PointerOffset(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pFieldOffset)
    {
        return _pEmitter->PointerOffset(pGlobal, pOffset, pFieldOffset);
    }

    llvm::Value* IRFunctionEmitter::ExtractStructField(llvm::Value* structValue, size_t fieldIndex)
    {
        return _pEmitter->ExtractStructField(structValue, fieldIndex);
    }

    llvm::Value* IRFunctionEmitter::GetStructFieldValue(llvm::Value* structPtr, size_t fieldIndex)
    {
        return Load(GetStructFieldPointer(structPtr, fieldIndex));
    }

    llvm::Value* IRFunctionEmitter::GetStructFieldPointer(llvm::Value* structPtr, size_t fieldIndex)
    {
        return _pEmitter->GetStructFieldPointer(structPtr, fieldIndex);
    }

    llvm::Value* IRFunctionEmitter::ValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset)
    {
        return Load(_pEmitter->PointerOffset(pGlobal, pOffset));
    }

    llvm::Value* IRFunctionEmitter::ValueAt(llvm::GlobalVariable* pGlobal, int offset)
    {
        return Load(_pEmitter->PointerOffset(pGlobal, Literal(offset)));
    }

    llvm::Value* IRFunctionEmitter::ValueAt(llvm::GlobalVariable* pGlobal)
    {
        return Load(_pEmitter->Pointer(pGlobal));
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

    llvm::Value* IRFunctionEmitter::ValueAt(llvm::Value* pPointer)
    {
        return ValueAt(pPointer, 0);
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

    // Control flow constructs
    void IRFunctionEmitter::For(size_t count, std::function<void(IRFunctionEmitter& function, llvm::Value* iterationVariable)> body)
    {
        auto loop = IRForLoopEmitter(*this);
        loop.Begin(count);
        body(*this, loop.LoadIterationVariable());
        loop.End();
    }

    void IRFunctionEmitter::For(llvm::Value* count, std::function<void(IRFunctionEmitter& function, llvm::Value* iterationVariable)> body)
    {
        auto loop = IRForLoopEmitter(*this);
        loop.Begin(count);
        body(*this, loop.LoadIterationVariable());
        loop.End();
    }

    void IRFunctionEmitter::For(size_t beginValue, size_t endValue, std::function<void(IRFunctionEmitter& function, llvm::Value* iterationVariable)> body)
    {
        For(beginValue, endValue, 1, body);
    }

    void IRFunctionEmitter::For(llvm::Value* beginValue, llvm::Value* endValue, std::function<void(IRFunctionEmitter& function, llvm::Value* iterationVariable)> body)
    {
        For(beginValue, endValue, Literal<int>(1), body);
    }

    void IRFunctionEmitter::For(size_t beginValue, size_t endValue, size_t increment, std::function<void(IRFunctionEmitter& function, llvm::Value* iterationVariable)> body)
    {
        auto loop = IRForLoopEmitter(*this);
        loop.Begin(beginValue, endValue, increment);
        body(*this, loop.LoadIterationVariable());
        loop.End();
    }

    void IRFunctionEmitter::For(llvm::Value* beginValue, llvm::Value* endValue, llvm::Value* increment, std::function<void(IRFunctionEmitter& function, llvm::Value* iterationVariable)> body)
    {
        auto loop = IRForLoopEmitter(*this);
        loop.Begin(beginValue, endValue, increment);
        body(*this, loop.LoadIterationVariable());
        loop.End();
    }

    void IRFunctionEmitter::While(llvm::Value* pTestValuePointer, std::function<void(IRFunctionEmitter& function)> body)
    {
        auto loop = IRWhileLoopEmitter(*this);
        loop.Begin(pTestValuePointer);
        body(*this);
        loop.End();
    }

    IRIfEmitter IRFunctionEmitter::If(llvm::Value* pTestValuePointer, std::function<void(IRFunctionEmitter& function)> body)
    {
        auto ifEmitter = IRIfEmitter(*this, true);
        ifEmitter.If(pTestValuePointer);
        {
            body(*this);
        }
        return ifEmitter;
    }

    // Deprecated versions
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

    IRWhileLoopEmitter IRFunctionEmitter::WhileLoop()
    {
        return IRWhileLoopEmitter(*this);
    }

    //
    // Individual async tasks
    //
    IRTask IRFunctionEmitter::StartAsyncTask(llvm::Function* taskFunction)
    {
        return StartAsyncTask(taskFunction, {});
    }

    IRTask IRFunctionEmitter::StartAsyncTask(IRFunctionEmitter& taskFunction)
    {
        return StartAsyncTask(taskFunction, {});
    }

    IRTask IRFunctionEmitter::StartAsyncTask(llvm::Function* taskFunction, const std::vector<llvm::Value*>& arguments)
    {
        return IRAsyncTask(*this, taskFunction, arguments);
    }

    IRTask IRFunctionEmitter::StartAsyncTask(IRFunctionEmitter& taskFunction, const std::vector<llvm::Value*>& arguments)
    {
        return IRAsyncTask(*this, taskFunction, arguments);
    }

    //
    // Array of tasks
    //
    IRTaskArray IRFunctionEmitter::StartTasks(IRFunctionEmitter& taskFunction, const std::vector<std::vector<llvm::Value*>>& arguments)
    {
        return StartTasks(taskFunction.GetFunction(), arguments);
    }

    IRTaskArray IRFunctionEmitter::StartTasks(llvm::Function* taskFunction, const std::vector<std::vector<llvm::Value*>>& arguments)
    {
        auto& compilerSettings = GetModule().GetCompilerOptions();
        if (compilerSettings.parallelize && compilerSettings.useThreadPool && !compilerSettings.targetDevice.IsWindows())
        {
            auto& threadPool = GetModule().GetThreadPool();
            return threadPool.AddTasks(*this, taskFunction, arguments);
        }
        else
        {
            std::vector<IRAsyncTask> tasks;
            for (const auto& arg : arguments)
            {
                tasks.push_back({ *this, taskFunction, arg });
            }
            return tasks;
        }
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
        IRValueList arguments = { Literal(size) };
        return CastPointer(Call(MallocFnName, arguments), type);
    }

    void IRFunctionEmitter::Free(llvm::Value* pValue)
    {
        Call(FreeFnName, CastPointer(pValue, VariableType::BytePointer));
    }

    llvm::Value* IRFunctionEmitter::Print(const std::string& text)
    {
        return Printf({ Literal(text) });
    }

    llvm::Value* IRFunctionEmitter::Printf(std::initializer_list<llvm::Value*> arguments)
    {
        EnsurePrintf();
        return Call(PrintfFnName, arguments);
    }

    llvm::Value* IRFunctionEmitter::Printf(const std::string& format, std::initializer_list<llvm::Value*> arguments)
    {
        EnsurePrintf();
        IRValueList callArgs;
        callArgs.push_back(_pEmitter->Literal(format));
        callArgs.insert(callArgs.end(), arguments);
        return Call(PrintfFnName, callArgs);
    }

    llvm::Value* IRFunctionEmitter::Printf(const std::string& format, std::vector<llvm::Value*> arguments)
    {
        EnsurePrintf();
        IRValueList callArgs;
        callArgs.push_back(_pEmitter->Literal(format));
        callArgs.insert(callArgs.end(), arguments.begin(), arguments.end());
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

    void IRFunctionEmitter::EnsurePrintf()
    {
        _pModuleEmitter->DeclarePrintf();
    }

    void IRFunctionEmitter::InsertMetadata(const std::string& tag, const std::string& content)
    {
        InsertMetadata(tag, std::vector<std::string>({ content }));
    }

    void IRFunctionEmitter::InsertMetadata(const std::string& tag, const std::vector<std::string>& content)
    {
        auto& context = GetLLVMContext();
        std::vector<llvm::Metadata*> metadataElements;
        for (const auto& value : content)
        {
            metadataElements.push_back({ llvm::MDString::get(context, value) });
        }
        auto metadataNode = llvm::MDNode::get(context, metadataElements);
        _pFunction->setMetadata(tag, metadataNode);
    }

    void IRFunctionEmitter::DotProduct(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination)
    {
        if (!pLeftValue->getType()->isPointerTy() || !pRightValue->getType()->isPointerTy() || !pDestination->getType()->isPointerTy())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers");
        }

        auto elementType = pLeftValue->getType()->getPointerElementType();
        if (elementType != pRightValue->getType()->getPointerElementType() || elementType != pDestination->getType()->getPointerElementType())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers to the same type");
        }

        StoreZero(pDestination);
        if (elementType->isFPOrFPVectorTy())
        {
            VectorOperator(TypedOperator::multiplyFloat, size, pLeftValue, pRightValue, [&pDestination, this](llvm::Value* i, llvm::Value* pValue) {
                OperationAndUpdate(pDestination, TypedOperator::addFloat, pValue);
            });
        }
        else if (elementType->isIntOrIntVectorTy())
        {
            VectorOperator(TypedOperator::multiply, size, pLeftValue, pRightValue, [&pDestination, this](llvm::Value* i, llvm::Value* pValue) {
                OperationAndUpdate(pDestination, TypedOperator::add, pValue);
            });
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers to integral or floating-point element types");
        }
    }

    void IRFunctionEmitter::DotProduct(llvm::Value* pSize, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination)
    {
        if (!pLeftValue->getType()->isPointerTy() || !pRightValue->getType()->isPointerTy() || !pDestination->getType()->isPointerTy())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers");
        }

        auto elementType = pLeftValue->getType()->getPointerElementType();
        if (elementType != pRightValue->getType()->getPointerElementType() || elementType != pDestination->getType()->getPointerElementType())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers to the same type");
        }

        StoreZero(pDestination);
        if (elementType->isFPOrFPVectorTy())
        {
            VectorOperator(TypedOperator::multiplyFloat, pSize, pLeftValue, pRightValue, [&pDestination, this](llvm::Value* i, llvm::Value* pValue) {
                OperationAndUpdate(pDestination, TypedOperator::addFloat, pValue);
            });
        }
        else if (elementType->isIntOrIntVectorTy())
        {
            VectorOperator(TypedOperator::multiply, pSize, pLeftValue, pRightValue, [&pDestination, this](llvm::Value* i, llvm::Value* pValue) {
                OperationAndUpdate(pDestination, TypedOperator::add, pValue);
            });
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers to integral or floating-point element types");
        }
    }

    llvm::Value* IRFunctionEmitter::DotProduct(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue)
    {
        if (!pLeftValue->getType()->isPointerTy() || !pRightValue->getType()->isPointerTy())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers");
        }

        auto elementType = pLeftValue->getType()->getPointerElementType();

        llvm::Value* pTotal = Variable(elementType, "result");
        DotProduct(size, pLeftValue, pRightValue, pTotal);
        return Load(pTotal);
    }

    //
    // BLAS functions
    //
    template <typename ValueType>
    void IRFunctionEmitter::CallGEMV(int m, int n, llvm::Value* A, int lda, llvm::Value* x, int incx, llvm::Value* y, int incy)
    {
        CallGEMV<ValueType>(m, n, static_cast<ValueType>(1.0), A, lda, x, incx, static_cast<ValueType>(0.0), y, incy);
    }

    template <typename ValueType>
    void IRFunctionEmitter::CallGEMV(int m, int n, ValueType alpha, llvm::Value* A, int lda, llvm::Value* x, int incx, ValueType beta, llvm::Value* y, int incy)
    {
        auto useBlas = CanUseBlas();
        llvm::Function* gemv = GetModule().GetRuntime().GetGEMVFunction<ValueType>(useBlas);
        if (gemv == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound, "Couldn't find GEMV function");
        }

        const auto CblasRowMajor = 101;
        const auto CblasNoTrans = 111;
        emitters::IRValueList args{ Literal(CblasRowMajor),
                                    Literal(CblasNoTrans), // transpose
                                    Literal(m),
                                    Literal(n),
                                    Literal(alpha),
                                    A,
                                    Literal(lda),
                                    x,
                                    Literal(incx),
                                    Literal(beta),
                                    y, // (output)
                                    Literal(incy) };
        Call(gemv, args);
    }

    template <typename ValueType>
    void IRFunctionEmitter::CallGEMM(int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc)
    {
        CallGEMM<ValueType>(false, false, m, n, k, A, lda, B, ldb, C, ldc);
    }

    template <typename ValueType>
    void IRFunctionEmitter::CallGEMM(bool transposeA, bool transposeB, int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc)
    {
        auto useBlas = CanUseBlas();
        llvm::Function* gemm = GetModule().GetRuntime().GetGEMMFunction<ValueType>(useBlas);
        if (gemm == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound, "Couldn't find GEMM function");
        }

        if (!useBlas && (transposeA || transposeB))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Transposed matrix multiply not currently implemented in non-blas codepath");
        }

        const auto CblasRowMajor = 101;
        const auto CblasNoTrans = 111;
        const auto CblasTrans = 112;

        emitters::IRValueList args{
            Literal(CblasRowMajor), // order
            Literal(transposeA ? CblasTrans : CblasNoTrans), // transposeA
            Literal(transposeB ? CblasTrans : CblasNoTrans), // transposeB
            Literal(m),
            Literal(n),
            Literal(k),
            Literal(static_cast<ValueType>(1.0)), // alpha
            A,
            Literal(lda), // lda
            B,
            Literal(ldb), // ldb
            Literal(static_cast<ValueType>(0.0)), // beta
            C, // C (output)
            Literal(ldc) // ldc
        };
        Call(gemm, args);
    }

    llvm::Value* IRFunctionEmitter::GetNumOpenBLASThreads()
    {
        auto getNumThreadsFunction = GetModule().GetRuntime().GetOpenBLASGetNumThreadsFunction();
        return Call(getNumThreadsFunction, {});
    }

    void IRFunctionEmitter::SetNumOpenBLASThreads(llvm::Value* numThreads)
    {
        auto setNumThreadsFunction = GetModule().GetRuntime().GetOpenBLASSetNumThreadsFunction();
        Call(setNumThreadsFunction, { numThreads });
    }

    //
    // Calling POSIX functions
    //

    bool IRFunctionEmitter::HasPosixFunctions() const
    {
        return true; // for now
    }

    llvm::Value* IRFunctionEmitter::PthreadCreate(llvm::Value* threadVar, llvm::Value* attrPtr, llvm::Function* taskFunction, llvm::Value* taskArgument)
    {
        auto selfFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCreateFunction();
        return Call(selfFunction, { threadVar, attrPtr, taskFunction, taskArgument });
    }

    llvm::Value* IRFunctionEmitter::PthreadEqual(llvm::Value* thread1, llvm::Value* thread2)
    {
        auto equalFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadEqualFunction();
        return Call(equalFunction, { thread1, thread2 });
    }

    void IRFunctionEmitter::PthreadExit(llvm::Value* status)
    {
        auto exitFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadExitFunction();
        Call(exitFunction, { status });
    }

    llvm::Value* IRFunctionEmitter::PthreadGetConcurrency()
    {
        auto getConcurrencyFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadGetConcurrencyFunction();
        return Call(getConcurrencyFunction, {});
    }

    llvm::Value* IRFunctionEmitter::PthreadDetach(llvm::Value* thread)
    {
        auto detachFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadDetachFunction();
        return Call(detachFunction, { thread });
    }

    llvm::Value* IRFunctionEmitter::PthreadJoin(llvm::Value* thread, llvm::Value* statusOut)
    {
        auto joinFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadJoinFunction();
        return Call(joinFunction, { thread, statusOut });
    }

    llvm::Value* IRFunctionEmitter::PthreadSelf()
    {
        auto selfFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadSelfFunction();
        return Call(selfFunction, {});
    }

    llvm::Value* IRFunctionEmitter::PthreadMutexInit(llvm::Value* mutexPtr, llvm::Value* attrPtr)
    {
        auto initFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexInitFunction();
        return Call(initFunction, { mutexPtr, attrPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadMutexDestroy(llvm::Value* mutexPtr)
    {
        auto destroyFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexDestroyFunction();
        return Call(destroyFunction, { mutexPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadMutexLock(llvm::Value* mutexPtr)
    {
        auto lockFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexLockFunction();
        return Call(lockFunction, { mutexPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadMutexTryLock(llvm::Value* mutexPtr)
    {
        auto trylockFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexTryLockFunction();
        return Call(trylockFunction, { mutexPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadMutexUnlock(llvm::Value* mutexPtr)
    {
        auto unlockFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexUnlockFunction();
        return Call(unlockFunction, { mutexPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadCondInit(llvm::Value* condPtr, llvm::Value* condAttrPtr)
    {
        auto initFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondInitFunction();
        return Call(initFunction, { condPtr, condAttrPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadCondDestroy(llvm::Value* condPtr)
    {
        auto destroyFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondDestroyFunction();
        return Call(destroyFunction, { condPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadCondWait(llvm::Value* condPtr, llvm::Value* mutexPtr)
    {
        auto waitFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondWaitFunction();
        return Call(waitFunction, { condPtr, mutexPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadCondTimedwait(llvm::Value* condPtr, llvm::Value* mutexPtr, llvm::Value* timespecPtr)
    {
        auto timedwaitFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondTimedwaitFunction();
        return Call(timedwaitFunction, { condPtr, mutexPtr, timespecPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadCondSignal(llvm::Value* condPtr)
    {
        auto signalFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondSignalFunction();
        return Call(signalFunction, { condPtr });
    }

    llvm::Value* IRFunctionEmitter::PthreadCondBroadcast(llvm::Value* condPtr)
    {
        auto broadcastFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondBroadcastFunction();
        return Call(broadcastFunction, { condPtr });
    }

    //
    // Experimental functions
    //

    llvm::Value* IRFunctionEmitter::GetCpu()
    {
        if (GetModule().GetCompilerOptions().targetDevice.IsLinux())
        {
            // Signature: int sched_getcpu(void);
            auto& context = GetLLVMContext();
            auto int32Type = llvm::Type::getInt32Ty(context);
            auto functionType = llvm::FunctionType::get(int32Type, {}, false);
            auto schedGetCpuFunction = static_cast<llvm::Function*>(GetModule().GetLLVMModule()->getOrInsertFunction("sched_getcpu", functionType));
            return Call(schedGetCpuFunction, {});
        }
        else
        {
            return Literal<int>(-1);
        }
    }

    //
    // Information about the current function begin emitted
    //

    IRBlockRegion* IRFunctionEmitter::AddRegion(llvm::BasicBlock* pBlock)
    {
        _pCurRegion = _regions.Add(pBlock);
        return _pCurRegion;
    }

    llvm::LLVMContext& IRFunctionEmitter::GetLLVMContext()
    {
        return _pModuleEmitter->GetLLVMContext();
    }

    IREmitter& IRFunctionEmitter::GetEmitter()
    {
        return *_pEmitter;
    }

    //
    // Serialization
    //

    void IRFunctionEmitter::WriteToStream(std::ostream& os)
    {
        llvm::raw_os_ostream out(os);
        _pFunction->print(out);
    }

    //
    // Metadata
    //
    void IRFunctionEmitter::IncludeInHeader()
    {
        _pFunction->setLinkage(llvm::GlobalValue::LinkageTypes::ExternalLinkage);
        InsertMetadata(c_declareFunctionInHeaderTagName);
    }

    void IRFunctionEmitter::IncludeInPredictInterface()
    {
        _pFunction->setLinkage(llvm::GlobalValue::LinkageTypes::ExternalLinkage);
        InsertMetadata(c_predictFunctionTagName);
    }

    void IRFunctionEmitter::IncludeInSwigInterface()
    {
        _pFunction->setLinkage(llvm::GlobalValue::LinkageTypes::ExternalLinkage);
        InsertMetadata(c_swigFunctionTagName);
    }


    //
    // Internal functions
    //

    llvm::Function* IRFunctionEmitter::ResolveFunction(const std::string& name)
    {
        llvm::Function* pFunction = GetLLVMModule()->getFunction(name);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return pFunction;
    }

    bool IRFunctionEmitter::CanUseBlas() const
    {
        return GetModule().GetCompilerOptions().useBlas;
    }

    //
    // IRFunctionCallArguments
    //
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
            _arguments.push_back(_functionEmitter.PointerOffset(pValue, 0));
        }
        else
        {
            _arguments.push_back(pValue);
        }
    }

    llvm::Value* IRFunctionCallArguments::AppendOutput(VariableType valueType, int size)
    {
        llvm::Value* pVector = _functionEmitter.Variable(valueType, size);
        Append(pVector);
        return pVector;
    }

    //
    // Explicit instantiations
    //
    template void IRFunctionEmitter::CallGEMV<float>(int m, int n, llvm::Value* A, int lda, llvm::Value* x, int incx, llvm::Value* y, int incy);

    template void IRFunctionEmitter::CallGEMV<float>(int m, int n, float alpha, llvm::Value* A, int lda, llvm::Value* x, int incx, float beta, llvm::Value* y, int incy);

    template void IRFunctionEmitter::CallGEMV<double>(int m, int n, llvm::Value* A, int lda, llvm::Value* x, int incx, llvm::Value* y, int incy);

    template void IRFunctionEmitter::CallGEMV<double>(int m, int n, double alpha, llvm::Value* A, int lda, llvm::Value* x, int incx, double beta, llvm::Value* y, int incy);

    template void IRFunctionEmitter::CallGEMM<float>(int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc);

    template void IRFunctionEmitter::CallGEMM<float>(bool transposeA, bool transposeB, int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc);

    template void IRFunctionEmitter::CallGEMM<double>(int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc);

    template void IRFunctionEmitter::CallGEMM<double>(bool transposeA, bool transposeB, int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc);
}
}
