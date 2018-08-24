////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionEmitter.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EmitterException.h"
#include "IRAsyncTask.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRFunctionEmitter.h"
#include "IRMetadata.h"
#include "IRModuleEmitter.h"
#include "IRParallelLoopEmitter.h"
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

    // Internal codes
    namespace
    {
        // Helper function for recursive function
        void MultiDimFor(IRFunctionEmitter& function, std::vector<IRFunctionEmitter::ConstLoopRange> ranges, std::vector<IRLocalScalar> prevIndices, IRFunctionEmitter::MultiDimForLoopBodyFunction body)
        {
            if (ranges.empty())
            {
                body(function, prevIndices);
            }
            else
            {
                auto range = ranges.front();
                std::vector<IRFunctionEmitter::ConstLoopRange> suffix(ranges.begin()+1, ranges.end());
                function.For(range.begin, range.end, [suffix, prevIndices, body](IRFunctionEmitter& function, auto index) {
                    std::vector<IRLocalScalar> prefix(prevIndices.begin(), prevIndices.end());
                    prefix.push_back(index);
                    MultiDimFor(function, suffix, prefix, body);
                });
            }
        }

        // Helper function for recursive function
        void MultiDimFor(IRFunctionEmitter& function, std::vector<IRFunctionEmitter::LoopRange> ranges, std::vector<IRLocalScalar> prevIndices, IRFunctionEmitter::MultiDimForLoopBodyFunction body)
        {
            if (ranges.empty())
            {
                body(function, prevIndices);
            }
            else
            {
                auto range = ranges.front();
                std::vector<IRFunctionEmitter::LoopRange> suffix(ranges.begin()+1, ranges.end());
                function.For(range.begin, range.end, [suffix, prevIndices, body](IRFunctionEmitter& function, auto index) {
                    std::vector<IRLocalScalar> prefix(prevIndices.begin(), prevIndices.end());
                    prefix.push_back(index);
                    MultiDimFor(function, suffix, prefix, body);
                });
            }
        }

        // Helper function for recursive function
        void TiledMultiDimFor(IRFunctionEmitter& function, std::vector<IRFunctionEmitter::ConstTiledLoopRange> ranges, std::vector<IRFunctionEmitter::BlockInterval> prevIntervals, IRFunctionEmitter::TiledMultiDimForLoopBodyFunction body)
        {
            if (ranges.empty())
            {
                body(function, prevIntervals);
            }
            else
            {
                auto range = ranges.front();
                std::vector<IRFunctionEmitter::ConstTiledLoopRange> suffix(ranges.begin()+1, ranges.end());
                function.For(range, [suffix, prevIntervals, body](IRFunctionEmitter& function, auto interval) {
                    std::vector<IRFunctionEmitter::BlockInterval> prefix(prevIntervals.begin(), prevIntervals.end());
                    prefix.push_back(interval);
                    TiledMultiDimFor(function, suffix, prefix, body);
                });
            }
        }

        // Helper function for recursive function
        void TiledMultiDimFor(IRFunctionEmitter& function, std::vector<IRFunctionEmitter::TiledLoopRange> ranges, std::vector<IRFunctionEmitter::BlockInterval> prevIntervals, IRFunctionEmitter::TiledMultiDimForLoopBodyFunction body)
        {
            if (ranges.empty())
            {
                body(function, prevIntervals);
            }
            else
            {
                auto range = ranges.front();
                std::vector<IRFunctionEmitter::TiledLoopRange> suffix(ranges.begin()+1, ranges.end());
                function.For(range, [suffix, prevIntervals, body](IRFunctionEmitter& function, auto interval) {
                    std::vector<IRFunctionEmitter::BlockInterval> prefix(prevIntervals.begin(), prevIntervals.end());
                    prefix.push_back(interval);
                    TiledMultiDimFor(function, suffix, prefix, body);
                });
            }
        }
    }

    //
    // IRFunctionEmitter methods
    //

    IRFunctionEmitter::IRFunctionEmitter(IRModuleEmitter* pModuleEmitter, IREmitter* pEmitter, LLVMFunction pFunction, const std::string& name)
        : _pModuleEmitter(pModuleEmitter), _pEmitter(pEmitter), _pFunction(pFunction), _name(name)
    {
        assert(_pModuleEmitter != nullptr);
        assert(_pEmitter != nullptr);
        assert(_pFunction != nullptr);
        SetUpFunction();
    }

    IRFunctionEmitter::IRFunctionEmitter(IRModuleEmitter* pModuleEmitter, IREmitter* pEmitter, LLVMFunction pFunction, const NamedVariableTypeList& arguments, const std::string& name)
        : IRFunctionEmitter(pModuleEmitter, pEmitter, pFunction, name)
    {
        // Note: already called other constructor
        RegisterFunctionArgs(arguments);
    }

    IRFunctionEmitter::IRFunctionEmitter(IRModuleEmitter* pModuleEmitter, IREmitter* pEmitter, LLVMFunction pFunction, const NamedLLVMTypeList& arguments, const std::string& name)
        : IRFunctionEmitter(pModuleEmitter, pEmitter, pFunction, name)
    {
        // Note: already called other constructor
        RegisterFunctionArgs(arguments);
    }

    void IRFunctionEmitter::CompleteFunction()
    {
        Verify();
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

    IRLocalPointer IRFunctionEmitter::LocalPointer(LLVMValue value)
    {
        return IRLocalPointer(*this, value);
    }

    IRLocalScalar IRFunctionEmitter::LocalScalar(LLVMValue value)
    {
        return IRLocalScalar(*this, value);
    }

    IRLocalScalar IRFunctionEmitter::LocalScalar()
    {
        return IRLocalScalar(*this, static_cast<LLVMValue>(nullptr));
    }

    IRLocalArray IRFunctionEmitter::LocalArray(LLVMValue value)
    {
        return IRLocalArray(*this, value);
    }

    IRLocalMatrix IRFunctionEmitter::LocalMatrix(LLVMValue value, const std::vector<int>& shape, std::array<int, 2> layout)
    {
        assert(shape.size() == 2);
        return IRLocalMatrix(*this, value, { shape[0], shape[1] }, layout);
    }

    IRLocalTensor IRFunctionEmitter::LocalTensor(LLVMValue value, const std::vector<int>& shape, std::array<int, 3> layout)
    {
        assert(shape.size() == 3);
        return IRLocalTensor(*this, value, { shape[0], shape[1], shape[2] }, layout);
    }

    IRLocalMultidimArray IRFunctionEmitter::LocalMultidimArray(LLVMValue value, std::initializer_list<int> dimensions)
    {
        return IRLocalMultidimArray(*this, value, dimensions);
    }

    LLVMValue IRFunctionEmitter::GetEmittedVariable(const VariableScope scope, const std::string& name)
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

    LLVMValue IRFunctionEmitter::GetFunctionArgument(const std::string& name)
    {
        return _locals.Get(name);
    }

    void IRFunctionEmitter::Verify()
    {
        llvm::verifyFunction(*_pFunction);
    }

    LLVMValue IRFunctionEmitter::LoadArgument(llvm::ilist_iterator<llvm::Argument>& argument)
    {
        return _pEmitter->Load(&(*argument));
    }

    LLVMValue IRFunctionEmitter::LoadArgument(llvm::Argument& argument)
    {
        return _pEmitter->Load(&argument);
    }

    LLVMValue IRFunctionEmitter::BitCast(LLVMValue pValue, VariableType valueType)
    {
        return _pEmitter->BitCast(pValue, valueType);
    }

    LLVMValue IRFunctionEmitter::BitCast(LLVMValue pValue, LLVMType valueType)
    {
        return _pEmitter->BitCast(pValue, valueType);
    }

    LLVMValue IRFunctionEmitter::CastPointer(LLVMValue pValue, LLVMType valueType)
    {
        if (!valueType->isPointerTy())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Type argument must be a pointer type");
        }
        return _pEmitter->CastPointer(pValue, valueType);
    }

    LLVMValue IRFunctionEmitter::CastPointer(LLVMValue pValue, VariableType valueType)
    {
        return _pEmitter->CastPointer(pValue, valueType);
    }

    LLVMValue IRFunctionEmitter::CastIntToPointer(LLVMValue pValue, LLVMType valueType)
    {
        assert(valueType->isPointerTy());
        return _pEmitter->CastIntToPointer(pValue, valueType);
    }

    LLVMValue IRFunctionEmitter::CastPointerToInt(LLVMValue pValue, LLVMType destinationType)
    {
        assert(pValue->getType()->isPointerTy());
        assert(destinationType->isIntOrIntVectorTy());
        return _pEmitter->CastPointerToInt(pValue, destinationType);
    }

    LLVMValue IRFunctionEmitter::CastIntToFloat(LLVMValue pValue, VariableType destinationType, bool isSigned)
    {
        assert(pValue->getType()->isIntOrIntVectorTy());
        return _pEmitter->CastIntToFloat(pValue, destinationType, isSigned);
    }

    LLVMValue IRFunctionEmitter::CastFloatToInt(LLVMValue pValue, VariableType destinationType)
    {
        assert(pValue->getType()->isFPOrFPVectorTy());
        return _pEmitter->CastFloatToInt(pValue, destinationType);
    }

    LLVMValue IRFunctionEmitter::CastBoolToByte(LLVMValue pValue)
    {
        return _pEmitter->CastInt(pValue, VariableType::Byte, false);
    }

    LLVMValue IRFunctionEmitter::CastBoolToInt(LLVMValue pValue)
    {
        return _pEmitter->CastInt(pValue, VariableType::Int32, false);
    }

    LLVMValue IRFunctionEmitter::Call(const std::string& name, LLVMValue pArgument)
    {
        LLVMFunction function = ResolveFunction(name);
        if (pArgument == nullptr)
        {
            return _pEmitter->Call(function);
        }
        return _pEmitter->Call(function, pArgument);
    }

    LLVMValue IRFunctionEmitter::Call(const std::string& name, const IRValueList& arguments)
    {
        return _pEmitter->Call(ResolveFunction(name), arguments);
    }

    LLVMValue IRFunctionEmitter::Call(const std::string& name, std::vector<IRLocalScalar> arguments)
    {
        return Call(ResolveFunction(name), arguments);
    }

    LLVMValue IRFunctionEmitter::Call(const std::string& name, std::initializer_list<LLVMValue> arguments)
    {
        return _pEmitter->Call(ResolveFunction(name), arguments);
    }

    LLVMValue IRFunctionEmitter::Call(IRFunctionEmitter& function, std::vector<LLVMValue> arguments)
    {
        assert(function.GetFunction() != nullptr);
        return _pEmitter->Call(function.GetFunction(), arguments);
    }

    LLVMValue IRFunctionEmitter::Call(LLVMFunction pFunction, std::initializer_list<LLVMValue> arguments)
    {
        assert(pFunction != nullptr);
        return _pEmitter->Call(pFunction, arguments);
    }

    LLVMValue IRFunctionEmitter::Call(LLVMFunction pFunction, std::vector<LLVMValue> arguments)
    {
        assert(pFunction != nullptr);
        return _pEmitter->Call(pFunction, arguments);
    }

    LLVMValue IRFunctionEmitter::Call(LLVMFunction pFunction, std::vector<IRLocalScalar> arguments)
    {
        assert(pFunction != nullptr);
        std::vector<LLVMValue> llvmArgs(arguments.begin(), arguments.end());
        return _pEmitter->Call(pFunction, llvmArgs);
    }

    void IRFunctionEmitter::Return()
    {
        _pEmitter->ReturnVoid();
    }

    LLVMValue IRFunctionEmitter::Return(LLVMValue value)
    {
        return _pEmitter->Return(value);
    }

    LLVMValue IRFunctionEmitter::Operator(UnaryOperationType type, LLVMValue value)
    {
        return _pEmitter->UnaryOperation(type, value);
    }

    LLVMValue IRFunctionEmitter::Operator(TypedOperator type, LLVMValue pLeftValue, LLVMValue pRightValue)
    {
        return _pEmitter->BinaryOperation(type, pLeftValue, pRightValue);
    }

    LLVMValue IRFunctionEmitter::Operator(TypedOperator type, llvm::iterator_range<llvm::Function::arg_iterator>& arguments)
    {
        auto values = arguments.begin();
        auto l = &(*values);
        ++values;
        auto r = &(*values);
        return Operator(type, l, r);
    }

    void IRFunctionEmitter::VectorOperator(TypedOperator type, size_t size, LLVMValue pLeftValue, LLVMValue pRightValue, std::function<void(LLVMValue, LLVMValue)> aggregator)
    {
        assert(pLeftValue != nullptr);
        assert(pRightValue != nullptr);

        For(size, [pLeftValue, pRightValue, type, aggregator](IRFunctionEmitter& fn, LLVMValue i) {
            auto pLeftItem = fn.ValueAt(pLeftValue, i);
            auto pRightItem = fn.ValueAt(pRightValue, i);
            auto pTemp = fn.Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        });
    }

    void IRFunctionEmitter::VectorOperator(TypedOperator type, LLVMValue pSize, LLVMValue pLeftValue, LLVMValue pRightValue, std::function<void(LLVMValue, LLVMValue)> aggregator)
    {
        assert(pSize != nullptr);
        assert(pLeftValue != nullptr);
        assert(pRightValue != nullptr);

        For(pSize, [pLeftValue, pRightValue, type, aggregator](IRFunctionEmitter& fn, LLVMValue i) {
            auto pLeftItem = fn.ValueAt(pLeftValue, i);
            auto pRightItem = fn.ValueAt(pRightValue, i);
            auto pTemp = fn.Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        });
    }

    void IRFunctionEmitter::VectorOperator(TypedOperator type, size_t size, LLVMValue pLeftValue, int leftStartAt, LLVMValue pRightValue, int rightStartAt, std::function<void(LLVMValue, LLVMValue)> aggregator)
    {
        assert(pLeftValue != nullptr);
        assert(pRightValue != nullptr);

        For(size, [pLeftValue, pRightValue, leftStartAt, rightStartAt, type, aggregator](IRFunctionEmitter& fn, LLVMValue i) {
            auto leftOffset = fn.Operator(TypedOperator::add, i, fn.Literal(leftStartAt));
            auto pLeftItem = fn.ValueAt(pLeftValue, leftOffset);
            auto rightOffset = fn.Operator(TypedOperator::add, i, fn.Literal(rightStartAt));
            auto pRightItem = fn.ValueAt(pRightValue, rightOffset);
            auto pTemp = fn.Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        });
    }

    void IRFunctionEmitter::Branch(llvm::BasicBlock* pDestinationBlock)
    {
        assert(pDestinationBlock != nullptr);
        _pEmitter->Branch(pDestinationBlock);
    }

    void IRFunctionEmitter::Branch(LLVMValue pConditionValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
    {
        _pEmitter->Branch(pConditionValue, pThenBlock, pElseBlock);
    }

    void IRFunctionEmitter::Branch(TypedComparison comparison, LLVMValue pValue, LLVMValue pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
    {
        LLVMValue pResult = Comparison(comparison, pValue, pTestValue);
        Branch(pResult, pThenBlock, pElseBlock);
    }

    void IRFunctionEmitter::Branch(LLVMValue pValue, bool testValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
    {
        return Branch(_pEmitter->Comparison(pValue, testValue), pThenBlock, pElseBlock);
    }

    LLVMValue IRFunctionEmitter::LogicalAnd(LLVMValue pTestValue1, LLVMValue pTestValue2)
    {
        assert(pTestValue1 != nullptr);
        assert(pTestValue2 != nullptr);
        return Operator(TypedOperator::logicalAnd, pTestValue1, pTestValue2);
    }

    LLVMValue IRFunctionEmitter::LogicalOr(LLVMValue pTestValue1, LLVMValue pTestValue2)
    {
        assert(pTestValue1 != nullptr);
        assert(pTestValue2 != nullptr);
        return Operator(TypedOperator::logicalOr, pTestValue1, pTestValue2);
    }

    LLVMValue IRFunctionEmitter::LogicalNot(LLVMValue pTestValue)
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

    LLVMValue IRFunctionEmitter::Comparison(TypedComparison type, LLVMValue pValue, LLVMValue pTestValue)
    {
        return _pEmitter->Comparison(type, pValue, pTestValue);
    }

    LLVMValue IRFunctionEmitter::Select(LLVMValue pCmp, LLVMValue pTrueValue, LLVMValue pFalseValue)
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

    llvm::AllocaInst* IRFunctionEmitter::Variable(LLVMType type, const std::string& namePrefix)
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

    llvm::AllocaInst* IRFunctionEmitter::Variable(VariableType type, int rows, int columns)
    {
        EntryBlockScope scope(*this);
        return _pEmitter->StackAllocate(type, rows, columns);
    }

    llvm::AllocaInst* IRFunctionEmitter::Variable(LLVMType type, int size)
    {
        EntryBlockScope scope(*this);
        return _pEmitter->StackAllocate(type, size);
    }

    llvm::AllocaInst* IRFunctionEmitter::Variable(LLVMType type, int rows, int columns)
    {
        EntryBlockScope scope(*this);
        return _pEmitter->StackAllocate(type, rows, columns);
    }

    LLVMValue IRFunctionEmitter::Load(LLVMValue pPointer)
    {
        auto result = _pEmitter->Load(pPointer);
        return result;
    }

    LLVMValue IRFunctionEmitter::Load(LLVMValue pPointer, const std::string& name)
    {
        return _pEmitter->Load(pPointer, name);
    }

    LLVMValue IRFunctionEmitter::Store(LLVMValue pPointer, LLVMValue pValue)
    {
        // check if we're a pointer to an array:
        auto pointerType = pPointer->getType();
        assert(pointerType->isPointerTy());
        auto pointedType = pointerType->getPointerElementType();
        if (pointedType->isArrayTy())
        {
            auto valueType = pointedType->getArrayElementType();
            auto dereferencedPointer = _pEmitter->DereferenceGlobalPointer(pPointer);
            auto castPointer = CastPointer(dereferencedPointer, valueType->getPointerTo());
            return SetValueAt(castPointer, 0, pValue);
        }

        return _pEmitter->Store(pPointer, pValue);
    }

    LLVMValue IRFunctionEmitter::StoreZero(LLVMValue pPointer, int numElements /* = 1 */)
    {
        assert(numElements >= 1);
        if(llvm::dyn_cast<llvm::GlobalVariable>(pPointer) != nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "StoreZero can't handle llvm::GlobalVariables");
        }

        LLVMValue returnValue = nullptr;
        auto type = llvm::cast<llvm::PointerType>(pPointer->getType())->getElementType();
        const int loopThresh = 16;
        if (numElements < loopThresh)
        {
            auto zero = llvm::Constant::getNullValue(type);
            for (int index = 0; index < numElements; ++index)
            {
                // We use SetValueAtA directly because SetValueAt tries to see if the variable
                // is a global variable first, which is not handled by this function.
                returnValue = SetValueAtA(pPointer, index, zero);
            }
        }
        else
        {
            auto typeSize = type->getPrimitiveSizeInBits() / 8;
            auto byteCount = static_cast<int>(typeSize * numElements);
            auto int8Type = llvm::Type::getInt8Ty(GetLLVMContext());
            auto bytePointer = CastPointer(pPointer, int8Type->getPointerTo());
            auto zero = llvm::ConstantInt::get(int8Type, 0);
            returnValue = _pEmitter->MemorySet(bytePointer, zero, Literal(byteCount));
        }

        return returnValue;
    }

    LLVMValue IRFunctionEmitter::OperationAndUpdate(LLVMValue pPointer, TypedOperator operation, LLVMValue pValue)
    {
        LLVMValue pNextVal = Operator(operation, Load(pPointer), pValue);
        Store(pPointer, pNextVal);
        return pNextVal;
    }

    LLVMValue IRFunctionEmitter::PtrOffsetA(LLVMValue pointer, int offset)
    {
        return _pEmitter->PointerOffset(pointer, Literal(offset));
    }

    LLVMValue IRFunctionEmitter::PtrOffsetA(LLVMValue pointer, LLVMValue pOffset, const std::string& name)
    {
        return _pEmitter->PointerOffset(pointer, pOffset, name);
    }

    LLVMValue IRFunctionEmitter::ValueAtA(LLVMValue pointer, int offset)
    {
        return _pEmitter->Load(PtrOffsetA(pointer, offset));
    }

    LLVMValue IRFunctionEmitter::ValueAtA(LLVMValue pointer, LLVMValue pOffset)
    {
        return _pEmitter->Load(PtrOffsetA(pointer, pOffset));
    }

    LLVMValue IRFunctionEmitter::SetValueAtA(LLVMValue pPointer, int offset, LLVMValue pValue)
    {
        return _pEmitter->Store(PtrOffsetA(pPointer, offset), pValue);
    }

    LLVMValue IRFunctionEmitter::SetValueAtA(LLVMValue pPointer, LLVMValue pOffset, LLVMValue pValue)
    {
        return _pEmitter->Store(PtrOffsetA(pPointer, pOffset), pValue);
    }

    void IRFunctionEmitter::FillStruct(LLVMValue structPtr, const std::vector<LLVMValue>& fieldValues)
    {
        auto& emitter = GetEmitter();
        auto& irBuilder = emitter.GetIRBuilder();
        for (size_t index = 0; index < fieldValues.size(); ++index)
        {
            auto field = irBuilder.CreateInBoundsGEP(structPtr, { Literal(0), Literal(static_cast<int>(index)) });
            Store(field, fieldValues[index]);
        }
    }

    LLVMValue IRFunctionEmitter::PtrOffsetH(LLVMValue pointer, int offset)
    {
        return PtrOffsetH(pointer, Literal(offset));
    }

    LLVMValue IRFunctionEmitter::PtrOffsetH(LLVMValue pointer, LLVMValue pOffset)
    {
        assert(pointer != nullptr);
        return _pEmitter->PointerOffset(Load(pointer), pOffset);
    }

    LLVMValue IRFunctionEmitter::ValueAtH(LLVMValue pointer, int offset)
    {
        return _pEmitter->Load(PtrOffsetH(pointer, offset));
    }

    LLVMValue IRFunctionEmitter::ValueAtH(LLVMValue pointer, LLVMValue pOffset)
    {
        return _pEmitter->Load(PtrOffsetH(pointer, pOffset));
    }

    LLVMValue IRFunctionEmitter::SetValueAtH(LLVMValue pPointer, int offset, LLVMValue pValue)
    {
        return _pEmitter->Store(PtrOffsetH(pPointer, offset), pValue);
    }

    LLVMValue IRFunctionEmitter::PointerOffset(llvm::GlobalVariable* pGlobal, LLVMValue pOffset)
    {
        return _pEmitter->PointerOffset(pGlobal, pOffset);
    }

    LLVMValue IRFunctionEmitter::PointerOffset(llvm::GlobalVariable* pGlobal, int offset)
    {
        return _pEmitter->PointerOffset(pGlobal, Literal(offset));
    }

    LLVMValue IRFunctionEmitter::PointerOffset(llvm::GlobalVariable* pGlobal, LLVMValue pOffset, LLVMValue pFieldOffset)
    {
        return _pEmitter->PointerOffset(pGlobal, pOffset, pFieldOffset);
    }

    LLVMValue IRFunctionEmitter::ExtractStructField(LLVMValue structValue, size_t fieldIndex)
    {
        return _pEmitter->ExtractStructField(structValue, fieldIndex);
    }

    LLVMValue IRFunctionEmitter::GetStructFieldValue(LLVMValue structPtr, size_t fieldIndex)
    {
        return Load(GetStructFieldPointer(structPtr, fieldIndex));
    }

    LLVMValue IRFunctionEmitter::GetStructFieldPointer(LLVMValue structPtr, size_t fieldIndex)
    {
        return _pEmitter->GetStructFieldPointer(structPtr, fieldIndex);
    }

    LLVMValue IRFunctionEmitter::ValueAt(llvm::GlobalVariable* pGlobal, LLVMValue pOffset)
    {
        return Load(_pEmitter->PointerOffset(pGlobal, pOffset));
    }

    LLVMValue IRFunctionEmitter::ValueAt(llvm::GlobalVariable* pGlobal, int offset)
    {
        return Load(_pEmitter->PointerOffset(pGlobal, Literal(offset)));
    }

    LLVMValue IRFunctionEmitter::ValueAt(llvm::GlobalVariable* pGlobal)
    {
        return Load(_pEmitter->DereferenceGlobalPointer(pGlobal));
    }

    LLVMValue IRFunctionEmitter::PointerOffset(LLVMValue pPointer, LLVMValue pOffset)
    {
        llvm::GlobalVariable* pGlobal = llvm::dyn_cast<llvm::GlobalVariable>(pPointer);
        if (pGlobal != nullptr)
        {
            return PointerOffset(pGlobal, pOffset);
        }
        return PtrOffsetA(pPointer, pOffset);
    }

    LLVMValue IRFunctionEmitter::PointerOffset(LLVMValue pPointer, int offset)
    {
        return PointerOffset(pPointer, Literal(offset));
    }

    LLVMValue IRFunctionEmitter::ValueAt(LLVMValue pPointer, LLVMValue pOffset)
    {
        llvm::GlobalVariable* pGlobal = llvm::dyn_cast<llvm::GlobalVariable>(pPointer);
        if (pGlobal != nullptr)
        {
            return ValueAt(pGlobal, pOffset);
        }
        return ValueAtA(pPointer, pOffset);
    }

    LLVMValue IRFunctionEmitter::ValueAt(LLVMValue pPointer, int offset)
    {
        return ValueAt(pPointer, Literal(offset));
    }

    LLVMValue IRFunctionEmitter::ValueAt(LLVMValue pPointer)
    {
        return ValueAt(pPointer, 0);
    }

    LLVMValue IRFunctionEmitter::SetValueAt(llvm::GlobalVariable* pGlobal, LLVMValue pOffset, LLVMValue pValue)
    {
        return Store(PointerOffset(pGlobal, pOffset), pValue);
    }

    LLVMValue IRFunctionEmitter::SetValueAt(LLVMValue pPointer, LLVMValue pOffset, LLVMValue pValue)
    {
        auto pointerType = pPointer->getType();
        assert(pointerType->isPointerTy());

        // check if we're a pointer to an array:
        auto pointedType = pointerType->getPointerElementType();
        if (pointedType->isArrayTy())
        {
            auto valueType = pointedType->getArrayElementType();
            auto dereferencedPointer = _pEmitter->DereferenceGlobalPointer(pPointer);
            auto castPointer = CastPointer(dereferencedPointer, valueType->getPointerTo());
            return SetValueAtA(castPointer, pOffset, pValue);
        }
        return SetValueAtA(pPointer, pOffset, pValue);
    }

    LLVMValue IRFunctionEmitter::SetValueAt(LLVMValue pPointer, int offset, LLVMValue pValue)
    {
        return SetValueAt(pPointer, Literal(offset), pValue);
    }

    // Control flow constructs

    //
    // For loops
    //
    void IRFunctionEmitter::For(int count, std::function<void(IRFunctionEmitter&, IRLocalScalar)> body)
    {
        if (count < 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "For loop count must be >= 0");
        }

        auto loop = IRForLoopEmitter(*this);
        loop.Begin(count);
        body(*this, LocalScalar(loop.LoadIterationVariable()));
        loop.End();
    }

    void IRFunctionEmitter::For(LLVMValue count, std::function<void(IRFunctionEmitter&, IRLocalScalar)> body)
    {
        auto loop = IRForLoopEmitter(*this);
        loop.Begin(count);
        body(*this, LocalScalar(loop.LoadIterationVariable()));
        loop.End();
    }

    void IRFunctionEmitter::For(int beginValue, int endValue, std::function<void(IRFunctionEmitter&, IRLocalScalar)> body)
    {
        if (endValue < beginValue)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "For loop begin must be <= end");
        }
        For(beginValue, endValue, 1, body);
    }

    void IRFunctionEmitter::For(LLVMValue beginValue, LLVMValue endValue, std::function<void(IRFunctionEmitter&, IRLocalScalar)> body)
    {
        For(beginValue, endValue, Literal<int>(1), body);
    }

    void IRFunctionEmitter::For(int beginValue, int endValue, int increment, std::function<void(IRFunctionEmitter&, IRLocalScalar)> body)
    {
        if (endValue < beginValue)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "For loop begin must be <= end");
        }
        auto loop = IRForLoopEmitter(*this);
        loop.Begin(beginValue, endValue, increment);
        body(*this, LocalScalar(loop.LoadIterationVariable()));
        loop.End();
    }

    void IRFunctionEmitter::For(LLVMValue beginValue, LLVMValue endValue, LLVMValue increment, std::function<void(IRFunctionEmitter&, IRLocalScalar)> body)
    {
        auto loop = IRForLoopEmitter(*this);
        loop.Begin(beginValue, endValue, increment);
        body(*this, LocalScalar(loop.LoadIterationVariable()));
        loop.End();
    }

    //
    // Extended for loops
    //

    void IRFunctionEmitter::For(const std::vector<ConstLoopRange>& ranges, MultiDimForLoopBodyFunction body)
    {
        emitters::MultiDimFor(*this, ranges, {}, body);
    }

    void IRFunctionEmitter::For(const std::vector<LoopRange>& ranges, MultiDimForLoopBodyFunction body)
    {
        emitters::MultiDimFor(*this, ranges, {}, body);
    }

    void IRFunctionEmitter::For(ConstTiledLoopRange range, TiledForLoopBodyFunction body)
    {
        auto stepSize = range.blockSize;
        auto numFullBlocks = (range.end-range.begin) / stepSize;
        auto fullBlocksEnd = range.begin + (numFullBlocks * stepSize);

        // full blocks
        if (numFullBlocks > 0)
        {
            // For(range.begin, fullBlocksEnd, stepSize, [stepSize, body](IRFunctionEmitter function, auto index) {
            For(numFullBlocks, [stepSize, range, body](IRFunctionEmitter function, auto blockIndex) {
                auto index = range.begin + blockIndex * stepSize;
                body(function, { index, index + stepSize, function.LocalScalar(stepSize), blockIndex });
            });
        }

        // epilogue -- with non-overlapping blocks, there can be at most one epilogue block
        if (fullBlocksEnd != range.end)
        {
            body(*this, { LocalScalar(fullBlocksEnd), LocalScalar(range.end), LocalScalar(range.end-fullBlocksEnd), LocalScalar(numFullBlocks) });
        }
    }

    void IRFunctionEmitter::For(TiledLoopRange range, TiledForLoopBodyFunction body)
    {
        if (!range.blockSize.IsConstantInt())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Tiled for loops must have a constant step size");
        }

        auto stepSize = range.blockSize.GetIntValue<int>();
        auto numFullBlocks = (range.end-range.begin) / stepSize;
        auto fullBlocksEnd = range.begin + (numFullBlocks * stepSize);

        // full blocks
        If(numFullBlocks > 0, [numFullBlocks, stepSize, range, body](auto& function) {
            function.For(numFullBlocks, range.blockSize, [stepSize, range, body](IRFunctionEmitter function, auto blockIndex) {
                auto index = range.begin + blockIndex * stepSize;
                body(function, { index, index + range.blockSize, range.blockSize, blockIndex });
            });
        });

        // epilogue -- with non-overlapping blocks, there can be at most one epilogue block
        If(fullBlocksEnd != range.end, [numFullBlocks, fullBlocksEnd, range, body](auto& function) {
            body(function, {fullBlocksEnd, range.end, range.end-fullBlocksEnd, numFullBlocks});
        });
    }

    void IRFunctionEmitter::For(const std::vector<ConstTiledLoopRange>& ranges, TiledMultiDimForLoopBodyFunction body)
    {
        emitters::TiledMultiDimFor(*this, ranges, {}, body);
    }

    void IRFunctionEmitter::For(const std::vector<TiledLoopRange>& ranges, TiledMultiDimForLoopBodyFunction body)
    {
        emitters::TiledMultiDimFor(*this, ranges, {}, body);
    }

    //
    // Parallel-for loops
    //
    void IRFunctionEmitter::ParallelFor(int count, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body)
    {
        if (count < 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "For loop count must be >= 0");
        }
        auto loop = IRParallelForLoopEmitter(*this);
        loop.EmitLoop(0, count, 1, { 0 }, capturedValues, body);
    }

    void IRFunctionEmitter::ParallelFor(int count, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body)
    {
        if (count < 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "For loop count must be >= 0");
        }
        auto loop = IRParallelForLoopEmitter(*this);
        loop.EmitLoop(0, count, 1, options, capturedValues, body);
    }

    void IRFunctionEmitter::ParallelFor(int beginValue, int endValue, int increment, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body)
    {
        if (endValue < beginValue)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "For loop begin must be <= end");
        }
        auto loop = IRParallelForLoopEmitter(*this);
        loop.EmitLoop(beginValue, endValue, increment, options, capturedValues, body);
    }

    void IRFunctionEmitter::ParallelFor(LLVMValue count, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body)
    {
        auto loop = IRParallelForLoopEmitter(*this);
        loop.EmitLoop(LocalScalar<int32_t>(0), LocalScalar(count), LocalScalar<int32_t>(1), { 0 }, capturedValues, body);
    }

    void IRFunctionEmitter::ParallelFor(LLVMValue count, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body)
    {
        auto loop = IRParallelForLoopEmitter(*this);
        loop.EmitLoop(LocalScalar<int32_t>(0), LocalScalar(count), LocalScalar<int32_t>(1), options, capturedValues, body);
    }

    void IRFunctionEmitter::ParallelFor(LLVMValue beginValue, LLVMValue endValue, LLVMValue increment, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body)
    {
        auto loop = IRParallelForLoopEmitter(*this);
        loop.EmitLoop(LocalScalar(beginValue), LocalScalar(endValue), LocalScalar(increment), options, capturedValues, body);
    }

    void IRFunctionEmitter::While(LLVMValue pTestValuePointer, std::function<void(IRFunctionEmitter& function)> body)
    {
        auto loop = IRWhileLoopEmitter(*this);
        loop.Begin(pTestValuePointer);
        body(*this);
        loop.End();
    }

    IRIfEmitter IRFunctionEmitter::If(LLVMValue pTestValuePointer, std::function<void(IRFunctionEmitter&)> body)
    {
        auto ifEmitter = IRIfEmitter(*this, true);
        ifEmitter.If(pTestValuePointer);
        {
            body(*this);
        }
        return ifEmitter;
    }

    IRIfEmitter IRFunctionEmitter::If(std::function<LLVMValue()> comparison, IfElseBodyFunction body)
    {
        auto ifEmitter = IRIfEmitter(*this, true);
        ifEmitter.If(comparison);
        {
            body(*this);
        }
        return ifEmitter;
    }

    IRIfEmitter IRFunctionEmitter::If(TypedComparison comparison, LLVMValue pValue, LLVMValue pTestValue, IfElseBodyFunction body)
    {
        auto ifEmitter = IRIfEmitter(*this, true);
        ifEmitter.If(comparison, pValue, pTestValue);
        {
            body(*this);
        }
        return ifEmitter;
    }

    //
    // Individual async tasks
    //
    IRTask IRFunctionEmitter::StartAsyncTask(LLVMFunction taskFunction)
    {
        return StartAsyncTask(taskFunction, {});
    }

    IRTask IRFunctionEmitter::StartAsyncTask(IRFunctionEmitter& taskFunction)
    {
        return StartAsyncTask(taskFunction, {});
    }

    IRTask IRFunctionEmitter::StartAsyncTask(LLVMFunction taskFunction, const std::vector<LLVMValue>& arguments)
    {
        return IRAsyncTask(*this, taskFunction, arguments);
    }

    IRTask IRFunctionEmitter::StartAsyncTask(IRFunctionEmitter& taskFunction, const std::vector<LLVMValue>& arguments)
    {
        return IRAsyncTask(*this, taskFunction, arguments);
    }

    //
    // Array of tasks
    //
    IRTaskArray IRFunctionEmitter::StartTasks(IRFunctionEmitter& taskFunction, const std::vector<std::vector<LLVMValue>>& arguments)
    {
        return StartTasks(taskFunction.GetFunction(), arguments);
    }

    IRTaskArray IRFunctionEmitter::StartTasks(LLVMFunction taskFunction, const std::vector<std::vector<LLVMValue>>& arguments)
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

    void IRFunctionEmitter::Optimize(IROptimizer& optimizer)
    {
        optimizer.OptimizeFunction(GetFunction());
    }

    LLVMValue IRFunctionEmitter::Malloc(VariableType type, int64_t size)
    {
        _pModuleEmitter->DeclareMalloc();
        IRValueList arguments = { Literal(size) };
        return CastPointer(Call(MallocFnName, arguments), type);
    }

    LLVMValue IRFunctionEmitter::Malloc(LLVMType type, int64_t size)
    {
        return Malloc(type, Literal(size));
    }

    LLVMValue IRFunctionEmitter::Malloc(LLVMType type, LLVMValue size)
    {
        _pModuleEmitter->DeclareMalloc();
        IRValueList arguments = { size };
        return CastPointer(Call(MallocFnName, arguments), type);
    }

    void IRFunctionEmitter::Free(LLVMValue pValue)
    {
        _pModuleEmitter->DeclareFree();
        Call(FreeFnName, CastPointer(pValue, VariableType::BytePointer));
    }

    LLVMValue IRFunctionEmitter::Print(const std::string& text)
    {
        return Printf({ Literal(text) });
    }

    LLVMValue IRFunctionEmitter::Printf(std::initializer_list<LLVMValue> arguments)
    {
        EnsurePrintf();
        return Call(PrintfFnName, arguments);
    }

    LLVMValue IRFunctionEmitter::Printf(const std::string& format, std::initializer_list<LLVMValue> arguments)
    {
        EnsurePrintf();
        IRValueList callArgs;
        callArgs.push_back(_pEmitter->Literal(format));
        callArgs.insert(callArgs.end(), arguments);
        return Call(PrintfFnName, callArgs);
    }

    LLVMValue IRFunctionEmitter::Printf(const std::string& format, std::vector<LLVMValue> arguments)
    {
        EnsurePrintf();
        IRValueList callArgs;
        callArgs.push_back(_pEmitter->Literal(format));
        callArgs.insert(callArgs.end(), arguments.begin(), arguments.end());
        return Call(PrintfFnName, callArgs);
    }

    void IRFunctionEmitter::PrintForEach(const std::string& formatString, LLVMValue pVector, int size)
    {
        EnsurePrintf();
        LLVMValue pFormat = Literal(formatString);
        For(size, [pVector, pFormat](IRFunctionEmitter& fn, LLVMValue i) {
            auto v = fn.ValueAt(pVector, i);
            fn.Printf({ pFormat, v });
        });
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

    void IRFunctionEmitter::DotProduct(int size, LLVMValue pLeftValue, LLVMValue pRightValue, LLVMValue pDestination)
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
            VectorOperator(TypedOperator::multiplyFloat, size, pLeftValue, pRightValue, [&pDestination, this](LLVMValue i, LLVMValue pValue) {
                OperationAndUpdate(pDestination, TypedOperator::addFloat, pValue);
            });
        }
        else if (elementType->isIntOrIntVectorTy())
        {
            VectorOperator(TypedOperator::multiply, size, pLeftValue, pRightValue, [&pDestination, this](LLVMValue i, LLVMValue pValue) {
                OperationAndUpdate(pDestination, TypedOperator::add, pValue);
            });
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers to integral or floating-point element types");
        }
    }

    void IRFunctionEmitter::DotProduct(LLVMValue pSize, LLVMValue pLeftValue, LLVMValue pRightValue, LLVMValue pDestination)
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
            VectorOperator(TypedOperator::multiplyFloat, pSize, pLeftValue, pRightValue, [&pDestination, this](LLVMValue i, LLVMValue pValue) {
                OperationAndUpdate(pDestination, TypedOperator::addFloat, pValue);
            });
        }
        else if (elementType->isIntOrIntVectorTy())
        {
            VectorOperator(TypedOperator::multiply, pSize, pLeftValue, pRightValue, [&pDestination, this](LLVMValue i, LLVMValue pValue) {
                OperationAndUpdate(pDestination, TypedOperator::add, pValue);
            });
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers to integral or floating-point element types");
        }
    }

    LLVMValue IRFunctionEmitter::DotProduct(int size, LLVMValue pLeftValue, LLVMValue pRightValue)
    {
        if (!pLeftValue->getType()->isPointerTy() || !pRightValue->getType()->isPointerTy())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Arguments to DotProduct must be pointers");
        }

        auto elementType = pLeftValue->getType()->getPointerElementType();

        LLVMValue pTotal = Variable(elementType, "result");
        DotProduct(size, pLeftValue, pRightValue, pTotal);
        return Load(pTotal);
    }

    //
    // BLAS functions
    //
    template <typename ValueType>
    void IRFunctionEmitter::CallGEMV(int m, int n, LLVMValue A, int lda, LLVMValue x, int incx, LLVMValue y, int incy)
    {
        CallGEMV<ValueType>(m, n, static_cast<ValueType>(1.0), A, lda, x, incx, static_cast<ValueType>(0.0), y, incy);
    }

    template <typename ValueType>
    void IRFunctionEmitter::CallGEMV(int m, int n, ValueType alpha, LLVMValue A, int lda, LLVMValue x, int incx, ValueType beta, LLVMValue y, int incy)
    {
        auto useBlas = CanUseBlas();
        LLVMFunction gemv = GetModule().GetRuntime().GetGEMVFunction<ValueType>(useBlas);
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
    void IRFunctionEmitter::CallGEMM(int m, int n, int k, LLVMValue A, int lda, LLVMValue B, int ldb, LLVMValue C, int ldc)
    {
        CallGEMM<ValueType>(false, false, m, n, k, A, lda, B, ldb, C, ldc);
    }

    template <typename ValueType>
    void IRFunctionEmitter::CallGEMM(bool transposeA, bool transposeB, int m, int n, int k, LLVMValue A, int lda, LLVMValue B, int ldb, LLVMValue C, int ldc)
    {
        auto useBlas = CanUseBlas();
        LLVMFunction gemm = GetModule().GetRuntime().GetGEMMFunction<ValueType>(useBlas);
        if (gemm == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound, "Couldn't find GEMM function");
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

    LLVMValue IRFunctionEmitter::GetNumOpenBLASThreads()
    {
        auto getNumThreadsFunction = GetModule().GetRuntime().GetOpenBLASGetNumThreadsFunction();
        return Call(getNumThreadsFunction, {});
    }

    void IRFunctionEmitter::SetNumOpenBLASThreads(LLVMValue numThreads)
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

    LLVMValue IRFunctionEmitter::PthreadCreate(LLVMValue threadVar, LLVMValue attrPtr, LLVMFunction taskFunction, LLVMValue taskArgument)
    {
        auto selfFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCreateFunction();
        return Call(selfFunction, { threadVar, attrPtr, taskFunction, taskArgument });
    }

    LLVMValue IRFunctionEmitter::PthreadEqual(LLVMValue thread1, LLVMValue thread2)
    {
        auto equalFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadEqualFunction();
        return Call(equalFunction, { thread1, thread2 });
    }

    void IRFunctionEmitter::PthreadExit(LLVMValue status)
    {
        auto exitFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadExitFunction();
        Call(exitFunction, { status });
    }

    LLVMValue IRFunctionEmitter::PthreadGetConcurrency()
    {
        auto getConcurrencyFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadGetConcurrencyFunction();
        return Call(getConcurrencyFunction, {});
    }

    LLVMValue IRFunctionEmitter::PthreadDetach(LLVMValue thread)
    {
        auto detachFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadDetachFunction();
        return Call(detachFunction, { thread });
    }

    LLVMValue IRFunctionEmitter::PthreadJoin(LLVMValue thread, LLVMValue statusOut)
    {
        auto joinFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadJoinFunction();
        return Call(joinFunction, { thread, statusOut });
    }

    LLVMValue IRFunctionEmitter::PthreadSelf()
    {
        auto selfFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadSelfFunction();
        return Call(selfFunction, {});
    }

    LLVMValue IRFunctionEmitter::PthreadMutexInit(LLVMValue mutexPtr, LLVMValue attrPtr)
    {
        auto initFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexInitFunction();
        return Call(initFunction, { mutexPtr, attrPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadMutexDestroy(LLVMValue mutexPtr)
    {
        auto destroyFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexDestroyFunction();
        return Call(destroyFunction, { mutexPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadMutexLock(LLVMValue mutexPtr)
    {
        auto lockFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexLockFunction();
        return Call(lockFunction, { mutexPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadMutexTryLock(LLVMValue mutexPtr)
    {
        auto trylockFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexTryLockFunction();
        return Call(trylockFunction, { mutexPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadMutexUnlock(LLVMValue mutexPtr)
    {
        auto unlockFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadMutexUnlockFunction();
        return Call(unlockFunction, { mutexPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadCondInit(LLVMValue condPtr, LLVMValue condAttrPtr)
    {
        auto initFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondInitFunction();
        return Call(initFunction, { condPtr, condAttrPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadCondDestroy(LLVMValue condPtr)
    {
        auto destroyFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondDestroyFunction();
        return Call(destroyFunction, { condPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadCondWait(LLVMValue condPtr, LLVMValue mutexPtr)
    {
        auto waitFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondWaitFunction();
        return Call(waitFunction, { condPtr, mutexPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadCondTimedwait(LLVMValue condPtr, LLVMValue mutexPtr, LLVMValue timespecPtr)
    {
        auto timedwaitFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondTimedwaitFunction();
        return Call(timedwaitFunction, { condPtr, mutexPtr, timespecPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadCondSignal(LLVMValue condPtr)
    {
        auto signalFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondSignalFunction();
        return Call(signalFunction, { condPtr });
    }

    LLVMValue IRFunctionEmitter::PthreadCondBroadcast(LLVMValue condPtr)
    {
        auto broadcastFunction = GetModule().GetRuntime().GetPosixEmitter().GetPthreadCondBroadcastFunction();
        return Call(broadcastFunction, { condPtr });
    }

    //
    // Experimental functions
    //

    LLVMValue IRFunctionEmitter::GetCpu()
    {
        if (GetModule().GetCompilerOptions().targetDevice.IsLinux())
        {
            // Signature: int sched_getcpu(void);
            auto& context = GetLLVMContext();
            auto int32Type = llvm::Type::getInt32Ty(context);
            auto functionType = llvm::FunctionType::get(int32Type, {}, false);
            auto schedGetCpuFunction = static_cast<LLVMFunction>(GetModule().GetLLVMModule()->getOrInsertFunction("sched_getcpu", functionType));
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

    LLVMFunction IRFunctionEmitter::ResolveFunction(const std::string& name)
    {
        LLVMFunction pFunction = GetLLVMModule()->getFunction(name);
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

    LLVMValue IRFunctionCallArguments::GetArgumentAt(size_t index) const
    {
        return _arguments[index];
    }

    void IRFunctionCallArguments::Append(LLVMValue pValue)
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

    LLVMValue IRFunctionCallArguments::AppendOutput(VariableType valueType, int size)
    {
        LLVMValue pVector = _functionEmitter.Variable(valueType, size);
        Append(pVector);
        return pVector;
    }

    //
    // Explicit instantiations
    //
    template void IRFunctionEmitter::CallGEMV<float>(int m, int n, LLVMValue A, int lda, LLVMValue x, int incx, LLVMValue y, int incy);

    template void IRFunctionEmitter::CallGEMV<float>(int m, int n, float alpha, LLVMValue A, int lda, LLVMValue x, int incx, float beta, LLVMValue y, int incy);

    template void IRFunctionEmitter::CallGEMV<double>(int m, int n, LLVMValue A, int lda, LLVMValue x, int incx, LLVMValue y, int incy);

    template void IRFunctionEmitter::CallGEMV<double>(int m, int n, double alpha, LLVMValue A, int lda, LLVMValue x, int incx, double beta, LLVMValue y, int incy);

    template void IRFunctionEmitter::CallGEMM<float>(int m, int n, int k, LLVMValue A, int lda, LLVMValue B, int ldb, LLVMValue C, int ldc);

    template void IRFunctionEmitter::CallGEMM<float>(bool transposeA, bool transposeB, int m, int n, int k, LLVMValue A, int lda, LLVMValue B, int ldb, LLVMValue C, int ldc);

    template void IRFunctionEmitter::CallGEMM<double>(int m, int n, int k, LLVMValue A, int lda, LLVMValue B, int ldb, LLVMValue C, int ldc);

    template void IRFunctionEmitter::CallGEMM<double>(bool transposeA, bool transposeB, int m, int n, int k, LLVMValue A, int lda, LLVMValue B, int ldb, LLVMValue C, int ldc);
}
}
